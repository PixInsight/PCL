// Main Testing module

#include <pjsr/Test.jsh>

#include "INDICCDControllerTests.js"

function INDICCDTestSuite()
{
   this.__base__ = Test;
   this.__base__( "INDICCDTestSuite" );

   this.deviceController = new INDIDeviceController;
   this.timer = new ElapsedTime;

   this.add( new INDICCDControllerTests( this ) );

   this.executeController = function()
   {
      if ( !this.deviceController.executeGlobal() )
         throw new Error( "INDIDeviceController.executeGlobal() failed" );
   };

   this.restartTimer = function()
   {
      this.timer.reset();
   };

   this.timeout = function()
   {
      if ( this.timer.value > 10 )
      {
         console.criticalln( "Timeout reached" );
         return true;
      }
      return false;
   };

   // N.B. begin() and end() methods reimplemented from Test

   this.begin = function()
   {
      // Connect to INDI server
      this.deviceController.serverConnect = true;
      this.executeController();

      // Wait until device names are received from server
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( indexOfDevice( (new INDIDeviceController).devices, CCD_DEVICE_NAME ) >= 0 )
            break;
      }

      // Wait for another 500 ms to allow for all device and property lists to
      // update completely.
      msleep( 500 );

      // Connect to CCD device
      let propertyKey = "/" + CCD_DEVICE_NAME + "/CONNECTION/CONNECT";
      this.deviceController.newProperties = [[propertyKey, "INDI_SWITCH", "ON"]];
      this.deviceController.serverCommand = "SET";
      this.executeController();
      this.deviceController.serverCommand = "";

      // Wait until device is connected
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( propertyEquals( (new INDIDeviceController).properties, propertyKey, "ON" ) )
            break;
      }
   };

   this.end = function()
   {
      // Disconnect from INDI server
      this.deviceController.serverConnect = false;
      this.executeController();
   };
};


#include "INDIMountControllerTests.js"

function INDIMountTestSuite()
{
   this.__base__ = Test;
   this.__base__( "INDIMountTestSuite" );

   this.deviceController = new INDIDeviceController;
   this.timer = new ElapsedTime;

   this.add( new INDIMountControllerTests( this ) );

   this.executeController = function()
   {
      if ( !this.deviceController.executeGlobal() )
         throw new Error( "INDIDeviceController.executeGlobal() failed" );
   };

   this.restartTimer = function()
   {
      this.timer.reset();
   };

   this.timeout = function()
   {
      if ( this.timer.value > 10 )
      {
         console.criticalln( "Timeout reached" );
         return true;
      }
      return false;
   };

   // N.B. begin() and end() methods reimplemented from Test

   this.begin = function()
   {
      // Connect to INDI server
      this.deviceController.serverConnect = true;
      this.executeController();

      // Wait until device names are received from server
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( indexOfDevice( (new INDIDeviceController).devices, MOUNT_DEVICE_NAME ) >= 0 )
            break;
      }

      // Wait for another 500 ms to allow for all device and property lists to
      // update completely.
      msleep( 500 );

      // Connect to Mount device
      let propertyKey = "/" + MOUNT_DEVICE_NAME + "/CONNECTION/CONNECT";
      this.deviceController.newProperties = [[propertyKey, "INDI_SWITCH", "ON"]];
      this.deviceController.serverCommand = "SET";
      this.executeController();
      this.deviceController.serverCommand = "";

      // Wait until device is connected
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( propertyEquals( (new INDIDeviceController).properties, propertyKey, "ON" ) )
            break;
      }
   };

   this.end = function()
   {
      // Disconnect from INDI server
      this.deviceController.serverConnect = false;
      this.executeController();
   };
};



// run CCD tests
//INDICCDTestSuite.prototype = new Test;

//(new INDICCDTestSuite).run();

// run Mount tests
INDIMountTestSuite.prototype = new Test;

(new INDIMountTestSuite).run();
