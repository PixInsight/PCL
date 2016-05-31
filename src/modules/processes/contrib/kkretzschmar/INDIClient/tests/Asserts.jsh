#ifndef _Asserts_jsh
#define _Asserts_jsh

function assertTrue(condition, message) {
    if (!condition) {
        message = message || "Assertion failed";
        if (typeof Error !== "undefined") {
            throw new Error(message);
        }
        throw message; // Fallback
    }
}

function assertEquals(expected,actual,message) {
   // check type of arguments
   assertTrue(typeof expected === typeof actual, message + ": type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual +"'")
   // check values
   assertTrue(expected===actual,message);
}

function expectTrue(condition) {
   assertTrue(condition,"condition violated: true != " +  condition);
}

function expectEquals(expected,actual) {
   // check type of arguments
   assertTrue(typeof expected === typeof actual, " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual +"'")
   // check values
   assertTrue(expected===actual,"Expected: '" + expected + "' != actual: '" + actual +"'");
}

function expectEqualsWithPrecision(expected,actual,precision) {
   // check type of arguments
   assertTrue(typeof expected === typeof actual, " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual +"'")
   // check values
   assertTrue(Math.abs(expected-actual) < precision,"Expected: |" + expected + " - " +  actual + "|  smaller than  " + precision );
}

function assertNotEquals(expected,actual,message) {
   // check type of arguments
   assertTrue(typeof expected === typeof actual, message + ": type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual +"'")
   // check values
   assertTrue(expected!==actual,message);
}

function expectNotEquals(expected,actual) {
   // check type of arguments
   assertTrue(typeof expected === typeof actual, " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual +"'")
   // check values
   assertTrue(expected!==actual,"Expected: '" + expected + "' == actual: '" + actual +"'");
}

function expectNotEqualsWithPrecision(expected,actual,precision) {
   // check type of arguments
   assertTrue(typeof expected === typeof actual, " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual +"'")
   // check values
   assertTrue(Math.abs(expected-actual) >= precision,"Expected: |" + expected + " - " +  actual + "|  larger than  " + precision );
}

#endif // _Asserts_jsh
