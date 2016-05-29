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
         var origPosRA=mountController.currentRightascension;
         var origPosDEC=mountController.currentDeclination;
         // set command type
         mountController.commandType=2; // MoveNorth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.commandType=3; // MoveNorth_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision(origPosRA,mountController.currentRightascension,0.1);
         expectTrue(origPosDEC < mountController.currentDeclination);
         origPosRA=mountController.currentRightascension;
         origPosDEC=mountController.currentDeclination;
         // set command type
         mountController.commandType=4; // MoveSouth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.commandType=5; // MoveSouth_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision(origPosRA,mountController.currentRightascension,0.1);
         expectTrue(origPosDEC > mountController.currentDeclination);
      }
   );

   this.add(
      function testMoveWestEast()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         var origPosRA=mountController.currentRightascension;
         var origPosDEC=mountController.currentDeclination;
         // set command type
         mountController.commandType=6; // MoveWest_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.commandType=7; // MoveWest_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectTrue(origPosRA < mountController.currentRightascension);
         expectEqualsWithPrecision(origPosDEC,mountController.currentDeclination,0.1);
         origPosRA=mountController.currentRightascension;
         origPosDEC=mountController.currentDeclination;
         // set command type
         mountController.commandType=8; // MoveEast_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.commandType=9; // MoveEst_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectTrue(origPosRA > mountController.currentRightascension);
         expectEqualsWithPrecision(origPosDEC,mountController.currentDeclination,0.1);
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
         mountController.targetRightascension=1;
         mountController.targetDeclination=15;
         mountController.commandType=10; // Goto
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision(1.0,mountController.currentRightascension,0.1);
         expectEqualsWithPrecision(15.0,mountController.currentDeclination,0.1);
      }
   );

   this.add(
      function testSynch()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         // set synch command
         mountController.commandType=11; // Synch
         mountController.targetRightascension=mountController.currentRightascension - 0.01;
         mountController.targetDeclination=mountController.currentDeclination + 0.01;
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision(mountController.targetRightascension,mountController.currentRightascension,0.1);
         expectEqualsWithPrecision(mountController.targetDeclination,mountController.currentDeclination,0.1);

         // restore original positions
         mountController.targetRightascension=mountController.currentRightascension + 0.01;
         mountController.targetDeclination=mountController.currentDeclination - 0.01;
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision(mountController.targetRightascension,mountController.currentRightascension,0.1);
         expectEqualsWithPrecision(mountController.targetDeclination,mountController.currentDeclination,0.1);
      }
   );

   this.add(
      function testParking()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // set park command
         mountController.commandType=1; // Park
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision(0.0,mountController.currentRightascension,0.1);
         expectEqualsWithPrecision(0.0,mountController.currentDeclination,0.1);
      }
   );

}

INDIMountControllerTests.prototype = new Test;
