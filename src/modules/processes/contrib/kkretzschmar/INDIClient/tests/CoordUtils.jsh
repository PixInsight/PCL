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
