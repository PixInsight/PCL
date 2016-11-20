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
         mountController.Command = 2; // MoveNorth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 3; // MoveNorth_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( origPosRA, mountController.currentRA, 0.1 );
         expectTrue( origPosDEC < mountController.currentDec );


         origPosRA = mountController.currentRA;
         origPosDEC = mountController.currentDec;
         // set command type
         mountController.Command = 4; // MoveSouth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 5; // MoveSouth_Stop
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
         mountController.Command = 6; // MoveWest_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 7; // MoveWest_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectTrue( origPosRA < mountController.currentRA );
         expectEqualsWithPrecision( origPosDEC, mountController.currentDec, 0.1 );
         origPosRA = mountController.currentRA;
         origPosDEC = mountController.currentDec;
         // set command type
         mountController.Command = 8; // MoveEast_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 9; // MoveEast_Stop
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
         mountController.Command = 10; // Goto
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
         mountController.Command = 11; // Sync
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
      function testAlignmentCorrection()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         mountController.alignmentModelFile="/home/klaus/data/Cpp/PCL/src/modules/processes/contrib/kkretzschmar/INDIClient/tests/OffsetTest.csv"
         mountController.AlignmentMethod=0;
         mountController.enableAlignmentCorrection=true;
         mountController.alignmentConfig=2;

         mountController.targetRA = 1.0;
         mountController.targetDec = 10;
         mountController.Command = 10; // Goto
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );

         var origPosRA = mountController.currentRA;
         var origPosDEC = mountController.currentDec;
         expectEqualsWithPrecision( mountController.targetRA - 0.01, mountController.currentRA, 0.1 );
         expectEqualsWithPrecision( mountController.targetDec - 0.02, mountController.currentDec, 0.1 );
      }
   );

   this.add(
      function testParking()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // set park command
         mountController.Command = 1; // Park
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
