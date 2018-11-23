// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// CoordUtils.jsh - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
//
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

#ifndef _CoordUtils_jsh
#define _CoordUtils_jsh

function ParseError(message) {
  this.message = message;
  this.name = "ParseError";
}

function sexadecimalStringToDouble(str,separator) {
   var tokens = str.split(separator);

   var n = tokens.length;
   if (n < 1){
      throw ParseException("empty string");
   }
   if (n > 3){
      throw ParseException("too many components");
   }
   var t1;
   if (n == 1){
      t1 = Math.abs(parseFloat(tokens[0]));
   } else {
      var d = Math.abs(parseInt(tokens[0]));
      if (n == 2){
         if ( m < 0  ){
          throw ParseError( "second component out of range", tokens[1] );
         }
         t1 = d + m/60;
      } else {
         var m = parseFloat(tokens[1]);
         if ( m < 0 ) {
            throw ParseError( "second component out of range", tokens[1] );
         }
         var s = parseFloat(tokens[2]);
         if ( s < 0 ) {
            throw ParseError( "third component out of range", tokens[2] );
         }
         t1 = d + (m + s/60)/60;
      }
   }
   return tokens[0].charAt(0) == "-" ?  -t1 : t1;
}

function myFrac(f) {
   return (f < 1.0) ? f : (f % Math.floor(f));
}

function sexadecimalStringFromDouble(d) {
   var s1 = Math.abs( d );
   var s2 = myFrac( s1 )*60;
   var s3 = myFrac( s2 )*60;
   s1 = Math.trunc( s1 );
   s2 = Math.trunc( s2 );
   s3 = Math.round(s3 * 1000) / 1000;


   return (d >=0 ? "+"  : "-") + s1 + ":" + s2 + ":" + s3;
}

function hourAngleRangeShift(hourAngle) {
    var shiftedHA = hourAngle;
    while ( shiftedHA < -12 )
        shiftedHA += 24;
    while ( shiftedHA >= 12 )
        shiftedHA -= 24;
    return shiftedHA;
}


#endif

// ----------------------------------------------------------------------------
// EOF CoordUtils.jsh - Released 2018-11-23T18:45:59Z
