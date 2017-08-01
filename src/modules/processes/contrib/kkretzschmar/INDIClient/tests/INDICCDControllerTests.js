// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// INDICCDControllerTests.js - Released 2017-08-01T14:26:58Z
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

#define CCD_DEVICE_NAME "CCD Simulator"

function INDICCDControllerTests( parent )
{
   this.__base__ = Test;
   this.__base__( "INDICCDControllerTests", parent );

   this.add(
      function testAcquireExposureDefault()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check that we have created one client image
         let window = ImageWindow.windowById( ccdController.clientFrames[0][0] );
         assertTrue( window && window.isWindow, "Image window not found" );
         console.writeln();
         // read and check fits keywords
         let fitskeys = window.keywords;
         assertTrue( fitskeys.length > 0, "No FITS keywords" );
         // check expected CCD device
         expectEquals( "'CCD Simulator'", fitskeys[indexOfFITSKeyword( fitskeys, "INSTRUME" )].value );
         // check image sizes
         let imgHeight = window.mainView.image.height;
         let imgWidth = window.mainView.image.width;
         expectEquals( imgWidth.toString(), fitskeys[indexOfFITSKeyword( fitskeys, "NAXIS1" )].value );
         expectEquals( imgHeight.toString(), fitskeys[indexOfFITSKeyword( fitskeys, "NAXIS2" )].value );
         // check EXPTIME
         expectEquals( ccdController.exposureTime.toString() + ".", fitskeys[indexOfFITSKeyword( fitskeys, "EXPTIME" )].value );
         // check XBINNING and YBINNING
         expectEquals( ccdController.binningX.toString(), fitskeys[indexOfFITSKeyword( fitskeys, "XBINNING" )].value );
         expectEquals( ccdController.binningY.toString(), fitskeys[indexOfFITSKeyword( fitskeys, "YBINNING" )].value );
         // check frame type
         expectEquals( "'Light'", fitskeys[indexOfFITSKeyword( fitskeys, "FRAME" )].value.replace( / /g, '' ) );
         // close image
         window.close();
      }
   );

   this.add(
      function testExposureTime()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.exposureTime = 2;
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check that we have created one client image
         let window = ImageWindow.windowById( ccdController.clientFrames[0][0] );
         assertTrue( window && window.isWindow, "Image window not found" );
         console.writeln();
         // read and check fits keywords
         let fitskeys = window.keywords;
         assertTrue( fitskeys.length > 0, "No FITS keywords" );
          // check EXPTIME
         expectEquals( ccdController.exposureTime.toString() + ".", fitskeys[indexOfFITSKeyword( fitskeys, "EXPTIME" )].value );
         // close image
         window.close();
      }
   );

   this.add(
      function testBinning()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.binningX = 2;
         ccdController.binningY = 2;
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check that we have created one client image
         let window = ImageWindow.windowById( ccdController.clientFrames[0][0] );
         assertTrue( window && window.isWindow, "Image window not found" );
         console.writeln();
         // read and check fits keywords
         let fitskeys = window.keywords;
         assertTrue( fitskeys.length > 0, "No FITS keywords" );
         // check XBINNING and YBINNING
         expectEquals( ccdController.binningX.toString(), fitskeys[indexOfFITSKeyword( fitskeys, "XBINNING" )].value );
         expectEquals( ccdController.binningY.toString(), fitskeys[indexOfFITSKeyword( fitskeys, "YBINNING" )].value );
         // close image
         window.close();
      }
   );

   this.add(
      function testFrameTypeBias()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.frameType = 1;
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check that we have created one client image
         let window = ImageWindow.windowById( ccdController.clientFrames[0][0] );
         assertTrue( window && window.isWindow, "Image window not found" );
         console.writeln();
         // read and check fits keywords
         let fitskeys = window.keywords;
         assertTrue( fitskeys.length > 0, "No FITS keywords" );
         // check frame type
         expectEquals( "'Bias'", fitskeys[indexOfFITSKeyword( fitskeys, "FRAME" )].value.replace( / /g, '' ) );
         // close image
         window.close();
      }
   );

   this.add(
      function testFrameTypeDark()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.frameType = 2;
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check that we have created one client image
         let window = ImageWindow.windowById( ccdController.clientFrames[0][0] );
         assertTrue( window && window.isWindow, "Image window not found" );
         console.writeln();
         // read and check fits keywords
         let fitskeys = window.keywords;
         assertTrue( fitskeys.length > 0, "No FITS keywords" );
         ccdController.exposureTime = 1;
         // check frame type
         expectEquals( "'Dark'", fitskeys[indexOfFITSKeyword( fitskeys, "FRAME" )].value.replace( / /g, '' ) );
         // close image
         window.close();
      }
   );

   this.add(
      function testFrameTypeFlat()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.frameType = 3;
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check that we have created one client image
         let window = ImageWindow.windowById( ccdController.clientFrames[0][0] );
         assertTrue( window && window.isWindow, "Image window not found" );
         console.writeln();
         // read and check fits keywordsMain
         let fitskeys = window.keywords;
         assertTrue( fitskeys.length > 0, "No FITS keywords" );
         // check frame type
         expectEquals( "'FlatField'", fitskeys[indexOfFITSKeyword( fitskeys, "FRAME" )].value.replace( / /g, '' ) );
         // close image
         window.close();
      }
   );

   this.add(
      function testExposureCount()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.exposureCount = 2;
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check that we have created two client images and close them
         expectEquals( ccdController.exposureCount, ccdController.clientFrames.length );
         ImageWindow.windowById( ccdController.clientFrames[0][0] ).close();
         ImageWindow.windowById( ccdController.clientFrames[1][0] ).close();
      }
   );

   this.add(
      function testuploadModeServer()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.serverUploadDirectory = Settings.readGlobal( "ImageWindow/DownloadsDirectory", DataType_UCString );
         ccdController.uploadMode = 1; // UploadMode_Server
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check uploaded file
         let defaultServerFilename = "LIGHT_B1x1_E1.000_001";
         let defaultServerFilePath = ccdController.serverUploadDirectory + '/' + defaultServerFilename + ".fits";
         expectEquals( true, ccdController.serverFrames[0][0] == defaultServerFilename );
         expectEquals( true, File.exists( defaultServerFilePath ) );
         // remove uploaded file
         File.remove( defaultServerFilePath );
      }
   );

   this.add(
      function testuploadModeServerAndClient()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.serverUploadDirectory = Settings.readGlobal( "ImageWindow/DownloadsDirectory", DataType_UCString );
         ccdController.uploadMode = 2; // UploadMode_ServerAndClient
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check uploaded file
         let defaultServerFilename = "LIGHT_B1x1_E1.000_001";
         let defaultServerFilePath = ccdController.serverUploadDirectory + '/' + defaultServerFilename + ".fits";
         expectEquals( true, ccdController.serverFrames[0][0] == defaultServerFilename );
         expectEquals( true, File.exists( defaultServerFilePath ) );
         // remove uploaded file
         File.remove( defaultServerFilePath );
         // check that we have created one client image and close it
         //assertTrue(false,"clientFrames: " + ccdController.clientFrames[0].toString())
         let window = ImageWindow.windowById( ccdController.clientFrames[0][0].toString() );
         assertTrue( window && window.isWindow, "Image window not found" );
         window.close();
      }
   );

   this.add(
      function testuploadServerFileNameTemplate()
      {
         let ccdController = new INDICCDFrame;
         ccdController.deviceName = CCD_DEVICE_NAME;
         ccdController.serverUploadDirectory = Settings.readGlobal( "ImageWindow/DownloadsDirectory", DataType_UCString );
         ccdController.uploadMode = 1; // UploadMode_Server
         ccdController.frameType = 1; // FrameType_Bias
         ccdController.exposureTime = 2;
         ccdController.exposureCount = 2;
         ccdController.serverFileNameTemplate = "MyObject_%f_B%b_E%e_%n";
         // execute in the global context
         assertTrue( ccdController.executeGlobal() );
         // check uploaded files
         let serverFileName1 = "MyObject_BIAS_B1x1_E2.000_001";
         let serverFilePath1 = ccdController.serverUploadDirectory + '/' + serverFileName1 + ".fits";
         let serverFileName2 = "MyObject_BIAS_B1x1_E2.000_002";
         let serverFilePath2 = ccdController.serverUploadDirectory + '/' + serverFileName2 + ".fits";
         expectEquals( true, ccdController.serverFrames[0][0] == serverFileName1 );
         expectEquals( true, File.exists( serverFilePath1 ) );
         expectEquals( true, ccdController.serverFrames[1][0] == serverFileName2 );
         expectEquals( true, File.exists( serverFilePath2 ) );
         // remove uploaded files
         File.remove( serverFilePath1 );
         File.remove( serverFilePath2 );
      }
   );
};

INDICCDControllerTests.prototype = new Test;

// ----------------------------------------------------------------------------
// EOF INDICCDControllerTests.js - Released 2017-08-01T14:26:58Z
