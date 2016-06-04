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
