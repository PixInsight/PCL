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
         assertTrue( mountController.executeGlobal() );
         expectEquals(0,mountController.currentDeclination);
         expectEquals(0,mountController.currentRightascension);
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
         expectEquals(1,mountController.currentRightascension);
         expectEquals(15,mountController.currentDeclination);
      }
   );
}

INDIMountControllerTests.prototype = new Test;
