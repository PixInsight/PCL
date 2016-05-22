#include <pjsr/Test.jsh>

#include "Asserts.jsh"
#include "INDI-helper.jsh"

#define MOUNT_DEVICE_NAME "Telescope Simulator"

function INDIMountControllerTests( parent )
{
   this.__base__ = Test;
   this.__base__( "INDIMountControllerTests", parent );

   this.add(
      function testGotoDefault()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context
         mountController.targetRightascension=0;
         mountController.targetDeclination=0;
         assertTrue( mountController.executeGlobal() );
         expectEqualsWithPrecision(0.0,mountController.currentDeclination,0.1);
         expectEqualsWithPrecision(0.0,mountController.currentRightascension,0.1);
      }
   );

   this.add(
      function testGotoSpecificCoordinates()
      {
         let mountController = new INDIMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // set target coordinates
         mountController.targetRightascension=1;
         mountController.targetDeclination=15;
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision(1.0,mountController.currentRightascension,0.1);
         expectEqualsWithPrecision(15.0,mountController.currentDeclination,0.1);
      }
   );
}

INDIMountControllerTests.prototype = new Test;
