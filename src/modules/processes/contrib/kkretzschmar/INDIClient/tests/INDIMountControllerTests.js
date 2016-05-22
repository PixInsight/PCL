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
      function testGoto()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         // set target coordinates
         mountController.targetRightascension=1;
         mountController.targetDeclination=15;
         mountController.commandType=2; // Goto
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
         mountController.commandType=3; // Synch
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
