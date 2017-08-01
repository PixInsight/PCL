//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/UCUppercase.cpp - Released 2017-08-01T14:23:38Z
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

/*
 * PCL Unicode uppercase character mapping routine.
 * Automatically generated on Fri, 20 Mar 2015 16:18:43 GMT
 */

#include <pcl/Defs.h>

namespace pcl
{

char16_type PCL_FUNC PCL_ToUppercase( char16_type c )
{
   if ( c < 962 )
   {
      if ( c < 592 )
      {
         if ( c < 414 )
         {
            if ( c < 331 )
            {
               if ( c < 255 )
               {
                  if ( c < 224 )
                  {
                     if ( c >= 97 && c <= 122 )
                        return c - 32;
                     if ( c == 181 )
                        return 924;
                  }
                  else
                  {
                     if ( c >= 224 && c <= 246 )
                        return c - 32;
                     if ( c >= 248 && c <= 254 )
                        return c - 32;
                  }
               }
               else
               {
                  if ( c < 305 )
                  {
                     if ( c == 255 )
                        return 376;
                     if ( c >= 257 && c <= 303 && (c & 1) != 0 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 305 )
                        return 73;
                     if ( c == 307 || c == 309 || c == 311 )
                        return c - 1;
                     if ( c >= 314 && c <= 328 && (c & 1) == 0 )
                        return c - 1;
                  }
               }
            }
            else
            {
               if ( c < 387 )
               {
                  if ( c < 383 )
                  {
                     if ( c >= 331 && c <= 375 && (c & 1) != 0 )
                        return c - 1;
                     if ( c == 378 || c == 380 || c == 382 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 383 )
                        return 83;
                     if ( c == 384 )
                        return 579;
                  }
               }
               else
               {
                  if ( c < 402 )
                  {
                     if ( c == 387 || c == 389 )
                        return c - 1;
                     if ( c == 392 || c == 396 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 402 || c == 409 )
                        return c - 1;
                     if ( c == 405 )
                        return 502;
                     if ( c == 410 )
                        return 573;
                  }
               }
            }
         }
         else
         {
            if ( c < 477 )
            {
               if ( c < 438 )
               {
                  if ( c < 424 )
                  {
                     if ( c == 414 || c == 891 )
                        return c + 130;
                     if ( c == 417 || c == 419 || c == 421 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 424 || c == 429 )
                        return c - 1;
                     if ( c == 432 || c == 436 )
                        return c - 1;
                  }
               }
               else
               {
                  if ( c < 447 )
                  {
                     if ( c == 438 || c == 441 )
                        return c - 1;
                     if ( c == 445 || c == 462 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 447 )
                        return 503;
                     if ( c == 454 || c == 457 || c == 460 )
                        return c - 2;
                     if ( c >= 464 && c <= 476 && (c & 1) == 0 )
                        return c - 1;
                  }
               }
            }
            else
            {
               if ( c < 507 )
               {
                  if ( c < 499 )
                  {
                     if ( c == 477 )
                        return 398;
                     if ( c >= 479 && c <= 495 && (c & 1) != 0 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 499 )
                        return 497;
                     if ( c == 501 || c == 505 )
                        return c - 1;
                  }
               }
               else
               {
                  if ( c < 572 )
                  {
                     if ( c >= 507 && c <= 543 && (c & 1) != 0 )
                        return c - 1;
                     if ( c >= 547 && c <= 563 && (c & 1) != 0 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 572 || c == 578 )
                        return c - 1;
                     if ( c == 575 || c == 576 )
                        return c + 10815;
                     if ( c >= 583 && c <= 591 && (c & 1) != 0 )
                        return c - 1;
                  }
               }
            }
         }
      }
      else
      {
         if ( c < 625 )
         {
            if ( c < 608 )
            {
               if ( c < 596 )
               {
                  if ( c < 594 )
                  {
                     if ( c == 592 )
                        return 11375;
                     if ( c == 593 )
                        return 11373;
                  }
                  else
                  {
                     if ( c == 594 )
                        return 11376;
                     if ( c == 595 )
                        return 385;
                  }
               }
               else
               {
                  if ( c < 601 )
                  {
                     if ( c == 596 )
                        return 390;
                     if ( c == 598 || c == 599 )
                        return c - 205;
                  }
                  else
                  {
                     if ( c == 601 )
                        return 399;
                     if ( c == 603 )
                        return 400;
                     if ( c == 604 )
                        return 42923;
                  }
               }
            }
            else
            {
               if ( c < 614 )
               {
                  if ( c < 611 )
                  {
                     if ( c == 608 )
                        return 403;
                     if ( c == 609 )
                        return 42924;
                  }
                  else
                  {
                     if ( c == 611 )
                        return 404;
                     if ( c == 613 )
                        return 42893;
                  }
               }
               else
               {
                  if ( c < 617 )
                  {
                     if ( c == 614 )
                        return 42922;
                     if ( c == 616 )
                        return 407;
                  }
                  else
                  {
                     if ( c == 617 || c == 623 )
                        return c - 211;
                     if ( c == 619 )
                        return 11362;
                     if ( c == 620 )
                        return 42925;
                  }
               }
            }
         }
         else
         {
            if ( c < 652 )
            {
               if ( c < 640 )
               {
                  if ( c < 629 )
                  {
                     if ( c == 625 )
                        return 11374;
                     if ( c == 626 )
                        return 413;
                  }
                  else
                  {
                     if ( c == 629 )
                        return 415;
                     if ( c == 637 )
                        return 11364;
                  }
               }
               else
               {
                  if ( c < 648 )
                  {
                     if ( c == 640 || c == 643 )
                        return c - 218;
                     if ( c == 647 )
                        return 42929;
                  }
                  else
                  {
                     if ( c == 648 )
                        return 430;
                     if ( c == 649 )
                        return 580;
                     if ( c == 650 || c == 651 )
                        return c - 217;
                  }
               }
            }
            else
            {
               if ( c < 887 )
               {
                  if ( c < 670 )
                  {
                     if ( c == 652 )
                        return 581;
                     if ( c == 658 )
                        return 439;
                  }
                  else
                  {
                     if ( c == 670 )
                        return 42928;
                     if ( c == 881 || c == 883 )
                        return c - 1;
                  }
               }
               else
               {
                  if ( c < 940 )
                  {
                     if ( c == 887 || c == 985 )
                        return c - 1;
                     if ( c == 892 || c == 893 )
                        return c + 130;
                  }
                  else
                  {
                     if ( c == 940 )
                        return 902;
                     if ( c == 941 || c == 942 || c == 943 )
                        return c - 37;
                     if ( c >= 945 && c <= 961 )
                        return c - 32;
                  }
               }
            }
         }
      }
   }
   else
   {
      if ( c < 8048 )
      {
         if ( c < 1121 )
         {
            if ( c < 987 )
            {
               if ( c < 976 )
               {
                  if ( c < 972 )
                  {
                     if ( c == 962 )
                        return 931;
                     if ( c >= 963 && c <= 971 )
                        return c - 32;
                  }
                  else
                  {
                     if ( c == 972 )
                        return 908;
                     if ( c == 973 || c == 974 )
                        return c - 63;
                  }
               }
               else
               {
                  if ( c < 981 )
                  {
                     if ( c == 976 )
                        return 914;
                     if ( c == 977 )
                        return 920;
                  }
                  else
                  {
                     if ( c == 981 )
                        return 934;
                     if ( c == 982 )
                        return 928;
                     if ( c == 983 )
                        return 975;
                  }
               }
            }
            else
            {
               if ( c < 1011 )
               {
                  if ( c < 1009 )
                  {
                     if ( c >= 987 && c <= 1007 && (c & 1) != 0 )
                        return c - 1;
                     if ( c == 1008 )
                        return 922;
                  }
                  else
                  {
                     if ( c == 1009 || c == 1104 )
                        return c - 80;
                     if ( c == 1010 || c == 8165 )
                        return c + 7;
                  }
               }
               else
               {
                  if ( c < 1016 )
                  {
                     if ( c == 1011 )
                        return 895;
                     if ( c == 1013 )
                        return 917;
                  }
                  else
                  {
                     if ( c == 1016 || c == 1019 )
                        return c - 1;
                     if ( c >= 1072 && c <= 1103 )
                        return c - 32;
                     if ( c >= 1105 && c <= 1119 )
                        return c - 80;
                  }
               }
            }
         }
         else
         {
            if ( c < 7835 )
            {
               if ( c < 1233 )
               {
                  if ( c < 1218 )
                  {
                     if ( c >= 1121 && c <= 1153 && (c & 1) != 0 )
                        return c - 1;
                     if ( c >= 1163 && c <= 1215 && (c & 1) != 0 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c >= 1218 && c <= 1230 && (c & 1) == 0 )
                        return c - 1;
                     if ( c == 1231 )
                        return 1216;
                  }
               }
               else
               {
                  if ( c < 7545 )
                  {
                     if ( c >= 1233 && c <= 1327 && (c & 1) != 0 )
                        return c - 1;
                     if ( c >= 1377 && c <= 1414 )
                        return c - 48;
                  }
                  else
                  {
                     if ( c == 7545 )
                        return 42877;
                     if ( c == 7549 )
                        return 11363;
                     if ( c >= 7681 && c <= 7829 && (c & 1) != 0 )
                        return c - 1;
                  }
               }
            }
            else
            {
               if ( c < 7968 )
               {
                  if ( c < 7936 )
                  {
                     if ( c == 7835 )
                        return 7776;
                     if ( c >= 7841 && c <= 7935 && (c & 1) != 0 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c >= 7936 && c <= 7943 )
                        return c + 8;
                     if ( c >= 7952 && c <= 7957 )
                        return c + 8;
                  }
               }
               else
               {
                  if ( c < 8000 )
                  {
                     if ( c >= 7968 && c <= 7975 )
                        return c + 8;
                     if ( c >= 7984 && c <= 7991 )
                        return c + 8;
                  }
                  else
                  {
                     if ( c >= 8000 && c <= 8005 )
                        return c + 8;
                     if ( c >= 8017 && c <= 8023 && (c & 1) != 0 )
                        return c + 8;
                     if ( c >= 8032 && c <= 8039 )
                        return c + 8;
                  }
               }
            }
         }
      }
      else
      {
         if ( c < 11365 )
         {
            if ( c < 8112 )
            {
               if ( c < 8058 )
               {
                  if ( c < 8054 )
                  {
                     if ( c == 8048 || c == 8049 )
                        return c + 74;
                     if ( c >= 8050 && c <= 8053 )
                        return c + 86;
                  }
                  else
                  {
                     if ( c == 8054 || c == 8055 )
                        return c + 100;
                     if ( c == 8056 || c == 8057 )
                        return c + 128;
                  }
               }
               else
               {
                  if ( c < 8064 )
                  {
                     if ( c == 8058 || c == 8059 )
                        return c + 112;
                     if ( c == 8060 || c == 8061 )
                        return c + 126;
                  }
                  else
                  {
                     if ( c >= 8064 && c <= 8071 )
                        return c + 8;
                     if ( c >= 8080 && c <= 8087 )
                        return c + 8;
                     if ( c >= 8096 && c <= 8103 )
                        return c + 8;
                  }
               }
            }
            else
            {
               if ( c < 8160 )
               {
                  if ( c < 8126 )
                  {
                     if ( c == 8112 || c == 8113 )
                        return c + 8;
                     if ( c == 8115 || c == 8131 )
                        return c + 9;
                  }
                  else
                  {
                     if ( c == 8126 )
                        return 921;
                     if ( c == 8144 || c == 8145 )
                        return c + 8;
                  }
               }
               else
               {
                  if ( c < 8526 )
                  {
                     if ( c == 8160 || c == 8161 )
                        return c + 8;
                     if ( c == 8179 )
                        return 8188;
                  }
                  else
                  {
                     if ( c == 8526 )
                        return 8498;
                     if ( c == 8580 || c == 11361 )
                        return c - 1;
                     if ( c >= 11312 && c <= 11358 )
                        return c - 48;
                  }
               }
            }
         }
         else
         {
            if ( c < 42563 )
            {
               if ( c < 11393 )
               {
                  if ( c < 11368 )
                  {
                     if ( c == 11365 )
                        return 570;
                     if ( c == 11366 )
                        return 574;
                  }
                  else
                  {
                     if ( c == 11368 || c == 11370 || c == 11372 )
                        return c - 1;
                     if ( c == 11379 || c == 11382 )
                        return c - 1;
                  }
               }
               else
               {
                  if ( c < 11507 )
                  {
                     if ( c >= 11393 && c <= 11491 && (c & 1) != 0 )
                        return c - 1;
                     if ( c == 11500 || c == 11502 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 11507 || c == 42561 )
                        return c - 1;
                     if ( c >= 11520 && c <= 11557 )
                        return c - 7264;
                     if ( c == 11559 || c == 11565 )
                        return c - 7264;
                  }
               }
            }
            else
            {
               if ( c < 42879 )
               {
                  if ( c < 42787 )
                  {
                     if ( c >= 42563 && c <= 42605 && (c & 1) != 0 )
                        return c - 1;
                     if ( c >= 42625 && c <= 42651 && (c & 1) != 0 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c >= 42787 && c <= 42799 && (c & 1) != 0 )
                        return c - 1;
                     if ( c >= 42803 && c <= 42863 && (c & 1) != 0 )
                        return c - 1;
                     if ( c == 42874 || c == 42876 )
                        return c - 1;
                  }
               }
               else
               {
                  if ( c < 42899 )
                  {
                     if ( c >= 42879 && c <= 42887 && (c & 1) != 0 )
                        return c - 1;
                     if ( c == 42892 || c == 42897 )
                        return c - 1;
                  }
                  else
                  {
                     if ( c == 42899 || c == 42903 )
                        return c - 1;
                     if ( c >= 42905 && c <= 42921 && (c & 1) != 0 )
                        return c - 1;
                     if ( c >= 65345 && c <= 65370 )
                        return c - 32;
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
// EOF pcl/UCUppercase.cpp - Released 2017-08-01T14:23:38Z
