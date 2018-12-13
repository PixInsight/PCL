// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// Asserts.jsh - Released 2018-12-12T09:25:25Z
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

#ifndef _Asserts_jsh
#define _Asserts_jsh

function assertTrue( condition, message )
{
   if ( !condition )
   {
      message = message || "Assertion failed";
      if ( typeof Error !== "undefined" )
         throw new Error( message );
      throw message; // Fallback
   }
}

function assertEquals( expected, actual, message )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               message + ": type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected === actual, message );
}

function expectTrue( condition )
{
   assertTrue( condition, "condition violated: true != " +  condition );
}

function expectEquals( expected, actual )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected === actual,
               "Expected: '" + expected + "' != actual: '" + actual + "'" );
}

function expectEqualsWithPrecision( expected, actual, precision )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( Math.abs( expected - actual ) < precision,
               "Expected: |" + expected + " - " +  actual + "|  smaller than  " + precision );
}

function assertNotEquals( expected, actual, message )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               message + ": type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected !== actual, message );
}

function expectNotEquals( expected, actual )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected !== actual,
               "Expected: '" + expected + "' == actual: '" + actual + "'" );
}

function expectNotEqualsWithPrecision( expected, actual, precision )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( Math.abs( expected - actual ) >= precision,
               "Expected: |" + expected + " - " +  actual + "|  larger than  " + precision );
}

function parseXTPMFile(fileName) {
   let result = {"GeoLat" : 0, "ModelConf": 0, "ModelParamWest" : [], "ModelParamEast" : []};

   let lines = File.readLines(fileName);
   for (var i = 0; i < lines.length ;i++) {
	   let line = lines[i];
	   let tokens1 = line.split('>');
	   if (tokens1.length > 1) {
		 let tokens2 = tokens1[1].split('<');
		 var xmlElement = tokens1[0];
	     var xmlValue = tokens2[0];
	     if (xmlElement.indexOf("GeographicLatitude") !== -1) {
	    	 result.GeoLat = parseFloat(xmlValue);
	     } else if (xmlElement.indexOf("Configuration") !== -1) {
	    	 result.ModelConf = parseInt(xmlValue);
	     } else if (xmlElement.indexOf("ModelParameters PierSide=\"West\"") !== -1) {
	    	 let params = xmlValue.split(',');
	    	 for (var j = 0; j < params.length; ++j){
	    		 result.ModelParamWest[j] = parseFloat(params[j]);
	    	 }
	     } else if (xmlElement.indexOf("ModelParameters PierSide=\"East\"") !== -1) {
	    	 let params = xmlValue.split(',');
	    	 for (var j = 0; j < params.length; ++j){
	    		 result.ModelParamEast[j] = parseFloat(params[j]);
	    	 }
	     }
       }
   }
   return result;
}


#endif // _Asserts_jsh

// ----------------------------------------------------------------------------
// EOF Asserts.jsh - Released 2018-12-12T09:25:25Z
