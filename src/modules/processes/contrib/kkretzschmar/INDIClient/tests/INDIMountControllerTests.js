// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// INDIMountControllerTests.js - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
//
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#include <pjsr/Test.jsh>

#include "Asserts.jsh"
#include "INDI-helper.jsh"

#define MOUNT_DEVICE_NAME "Telescope Simulator"

function INDIMountControllerTests( parent )
{
   this.__base__ = Test;
   this.__base__( "INDIMountControllerTests", parent );

   this.add(
      function testUnpark()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
      }
   );

   this.add(
      function testMoveNorthSouth()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         var origPosRA = mountController.currentRA;
         var origPosDEC = mountController.currentDec;
         // set command type
         mountController.command = 2; // MoveNorth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.command = 3; // MoveNorth_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( origPosRA, mountController.currentRA, 0.1 );
         expectTrue( origPosDEC < mountController.currentDec );
         origPosRA = mountController.currentRA;
         origPosDEC = mountController.currentDec;
         // set command type
         mountController.command = 4; // MoveSouth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.command = 5; // MoveSouth_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( origPosRA, mountController.currentRA, 0.1 );
         expectTrue( origPosDEC > mountController.currentDec );
      }
   );

   this.add(
      function testMoveWestEast()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         var origPosRA = mountController.currentRA;
         var origPosDEC = mountController.currentDec;
         // set command type
         mountController.command = 6; // MoveWest_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.command = 7; // MoveWest_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectTrue( origPosRA < mountController.currentRA );
         expectEqualsWithPrecision( origPosDEC, mountController.currentDec, 0.1 );
         origPosRA = mountController.currentRA;
         origPosDEC = mountController.currentDec;
         // set command type
         mountController.command = 8; // MoveEast_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.command = 9; // MoveEast_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectTrue( origPosRA > mountController.currentRA );
         expectEqualsWithPrecision( origPosDEC, mountController.currentDec, 0.1 );
      }
   );

   this.add(
      function testGoto()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         // set target coordinates
         mountController.targetRA = 1;
         mountController.targetDec = 15;
         mountController.command = 10; // Goto
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( 1.0, mountController.currentRA, 0.1 );
         expectEqualsWithPrecision( 15.0, mountController.currentDec, 0.1 );
      }
   );

   this.add(
      function testSync()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         // set synch command
         mountController.command = 11; // Sync
         mountController.targetRA = mountController.currentRA - 0.01;
         mountController.targetDec = mountController.currentDec + 0.01;
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( mountController.targetRA, mountController.currentRA, 0.1 );
         expectEqualsWithPrecision( mountController.targetDec, mountController.currentDec, 0.1 );

         // restore original positions
         mountController.targetRA = mountController.currentRA + 0.01;
         mountController.targetDec = mountController.currentDec - 0.01;
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( mountController.targetRA, mountController.currentRA, 0.1 );
         expectEqualsWithPrecision( mountController.targetDec, mountController.currentDec, 0.1 );
      }
   );

   this.add(
      function testParking()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // set park command
         mountController.command = 1; // Park
         mountController.targetRA = 0;
         mountController.targetDec = +90;
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( 0.0, mountController.currentRA, 0.1 );
         expectEqualsWithPrecision( +90.0, mountController.currentDec, 0.1 );
      }
   );
}

INDIMountControllerTests.prototype = new Test;

// ----------------------------------------------------------------------------
// EOF INDIMountControllerTests.js - Released 2017-05-02T09:43:01Z
