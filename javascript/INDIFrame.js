#include <pjsr/TextAlign.jsh>
#include <pjsr/FrameStyle.jsh>
#include <pjsr/Sizer.jsh>
#include <pjsr/StdIcon.jsh>
#include <pjsr/StdCursor.jsh>
#include <pjsr/StarDetector.jsh>
#include "IndiClient.jsh"
#include "INDIProperties-gui.jsh"

#define USE_SOLVER_LIBRARY
#define TITLE "Image Solver"
#define SETTINGS_MODULE "SOLVER"
#define STAR_CSV_FILE   File.systemTempDirectory + "/stars.csv"
#include "ImageSolver.js"
#include "WCSmetadata.jsh"
#include "AstronomicalCatalogs.jsh"
#include "SearchCoordinatesDialog.js"

var indi = new INDIclient();



function mean(array){
   var totalValue=0;
   for (var i=0; i<array.length; ++i){
      totalValue=totalValue+array[i].size;
   }
   return totalValue/array.length;
}

function max(starArray){
   var maxSize=0;
   var maxPos=[0,0];
   for (var i=0; i<starArray.length; ++i){
      if (starArray[i].size > maxSize){
         maxSize=starArray[i].size;
         maxPos=starArray[i].pos;
      }
   }
   return [maxSize,maxPos];
}


function dumpStarSizes(starSizeMeans){
   console.writeln( "Exposure#   Star size    Tendency       star pos         #stars" );
   console.writeln( "=========   =========    ========     =============      ======" );
   var tendency="o";
   for (var i=0; i<starSizeMeans.length; ++i){
     if (i>0 && Math.abs(starSizeMeans[i][0]-starSizeMeans[i-1][0])>5){
       if (starSizeMeans[i][0] > starSizeMeans[i-1][0]){
          tendency="+";
       }
       else {
          tendency="-";
       }
     }

     console.writeln( format( "%4u        %6.2f           %ls         (%4u,%4u)       %4u ",
                                i, starSizeMeans[i][0], tendency,starSizeMeans[i][1].x,starSizeMeans[i][1].y, starSizeMeans[i][2] ) );
    }
}

function convertToHMS(floatNum){
   var H=Math.trunc(floatNum);
   var M=Math.trunc((floatNum-H)*60);
   var S=Math.trunc(((floatNum-H)*60 - M)*60);
   return [H,M,S];
}




SetPropertyDialog.prototype = new Dialog;

function mainDialog()
{
   this.__base__ = Dialog;
   this.__base__();

   this.INDIAsynchActionEnum = {
      None:0,
      ServerConnect :1,
      ServerDisconnect :2,
      TelescopeConnect : 3,
      TelescopeDisconnect : 4
   }

   this.INDIAsynchAction = this.INDIAsynchActionEnum.None;

   var ttStr = "";
   var selectedPropertyNode = TreeBoxNode;

   // BEGIN of INDI server connection:  ========================================================
   this.INDIServerHost_Label = new labelBox(this, "INDI server host:", TextAlign_VertCenter, 120);
   //this.INDIServerHost_Edit = new editBox(this, "raspberrypi", false, FrameStyle_Box, 50);
   this.INDIServerHost_Edit = new editBox(this, "192.168.56.1", false, FrameStyle_Box, 50);
   //this.INDIServerHost_Edit.value="raspberrypi";
   this.INDIServerHost_Edit.value="192.168.56.1";
   this.INDIServerHost_Edit.setFixedWidth(80);
   ttStr = "INDI server host name."
   this.INDIServerHost_Edit.toolTip = ttStr;
   this.INDIServerHost_Edit.onTextUpdated = function(text)
   {
      indi.setServerHost(text);
   }
   // set server host to default value
   indi.setServerHost(this.INDIServerHost_Edit.value);


   this.INDIServerPort_Label = new labelBox(this, "port:", TextAlign_VertCenter, 120);
   this.INDIServerPort_Edit = new spinBox(this, 7624, 0,9999, 1, true);
   this.INDIServerPort_Edit.setFixedWidth(80);
   this.INDIServerPort_Edit.value=7624;
   ttStr = "INDI server port."
   this.INDIServerPort_Edit.toolTip = ttStr;
   this.INDIServerPort_Edit.onValueUpdated = function(portNumber)
   {
      indi.setServerPort(portNumber);
   }

   ttStr = "Connect to INDI server."
   this.INDIConnect_PushButton = new pushButton(this, "Connect", "", ttStr);
   this.INDIConnect_PushButton.onClick = function()
   {
      indi.connectServer();
      this.dialog.INDIAsynchAction =this.dialog.INDIAsynchActionEnum.ServerConnect;
      timer.start();

   }

   ttStr = "Disconnect from INDI server."
   this.INDIDisconnect_PushButton = new pushButton(this, "Disconnect", "", ttStr);
   this.INDIDisconnect_PushButton.onClick = function()
   {
      indi.disconnectServer();
   }

   this.INDIServer_HSizer = new HorizontalSizer;
   with(this.INDIServer_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDIServerHost_Label);
      add(this.INDIServerHost_Edit);
      add(this.INDIServerPort_Label);
      add(this.INDIServerPort_Edit);
      addStretch();
      add(this.INDIConnect_PushButton);
      add(this.INDIDisconnect_PushButton);
   }
   // End of INDI server connection:  ========================================================
   this.INDICamera_Label = new labelBox(this, "CCD camera:", TextAlign_VertCenter, 120);
   this.INDICamera_Label.setFixedWidth(100);
   this.INDICamera_Combo = new ComboBox(this);
   this.INDICamera_Combo.editEnabled = false;
   this.INDICamera_Combo.setMinWidth(150);
   var ttStr = "Select CCD camera device";
   this.INDICamera_Combo.toolTip = ttStr;
   this.INDICamera_Combo.onItemSelected = function ()
   {
      currentCamera=this.dialog.INDICamera_Combo.itemText(this.dialog.INDICamera_Combo.currentItem);
   }

   ttStr = "Connect to INDI camera."
   this.INDICameraConnect_PushButton = new pushButton(this, "Connect", "", ttStr);
   this.INDICameraConnect_PushButton.onClick = function()
   {
      indi.sendNewProperty("/" + currentCamera + "/CONNECTION/CONNECT","INDI_SWITCH","ON");
      //this.dialog.INDICamera_Combo.update();
   }

   ttStr = "Disconnect INDI camera."
   this.INDICameraDisconnect_PushButton = new pushButton(this, "Disconnect", "", ttStr);
   this.INDICameraDisconnect_PushButton.onClick = function()
   {
      indi.sendNewProperty("/" + currentCamera + "/CONNECTION/DISCONNECT","INDI_SWITCH","ON");
   }

   this.INDICamera_HSizer = new HorizontalSizer;
   with(this.INDICamera_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDICamera_Label);
      add(this.INDICamera_Combo);
      addStretch();
      add(this.INDICameraConnect_PushButton);
      add(this.INDICameraDisconnect_PushButton);
   }



   this.INDITelescope_Label = new labelBox(this, "Telescope:", TextAlign_VertCenter, 120);
   this.INDITelescope_Label.setFixedWidth(100);
   this.INDITelescope_Combo = new ComboBox(this);
   this.INDITelescope_Combo.editEnabled = false;
   this.INDITelescope_Combo.setMinWidth(150);
   var ttStr = "Select telescope device";
   this.INDITelescope_Combo.toolTip = ttStr;
   this.INDITelescope_Combo.onItemSelected = function ()
   {
      currentTelescope=this.dialog.INDITelescope_Combo.itemText(this.dialog.INDITelescope_Combo.currentItem);
   }

   ttStr = "Connect to INDI telescope."
   this.INDITelescopeConnect_PushButton = new pushButton(this, "Connect", "", ttStr);
   this.INDITelescopeConnect_PushButton.onClick = function()
   {
      indi.sendNewProperty("/" + currentTelescope + "/CONNECTION/CONNECT","INDI_SWITCH","ON");
      this.dialog.INDIAsynchAction =this.dialog.INDIAsynchActionEnum.TelescopeConnect;
      timer.start();
   }

   ttStr = "Disconnect INDI telescope."
   this.INDITelescopeDisconnect_PushButton = new pushButton(this, "Disconnect", "", ttStr);
   this.INDITelescopeDisconnect_PushButton.onClick = function()
   {
      indi.sendNewProperty("/" + currentTelescope + "/CONNECTION/DISCONNECT","INDI_SWITCH","ON");
   }

   this.INDITelescope_HSizer = new HorizontalSizer;
   with(this.INDITelescope_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDITelescope_Label);
      add(this.INDITelescope_Combo);
      addStretch();
      add(this.INDITelescopeConnect_PushButton);
      add(this.INDITelescopeDisconnect_PushButton);


   }




   // BEGIN of frame aquisition:  ========================================================
   this.ExposureDuration_Label = new labelBox(this, "Exposure duration:", TextAlign_VertCenter, 120);
   this.ExposureDuration_Edit = new editBox(this, "", false, FrameStyle_Box, 50);
   this.ExposureDuration_Edit.setFixedWidth(80);
   ttStr = "Exposure duration."
   this.ExposureDuration_Edit.toolTip = ttStr;
   this.ExposureDuration_Edit.onTextUpdated = function(text)
   {
      expDuration=text;
   }

   this.ExposureNumber_Label = new labelBox(this, "Number of exposures:", TextAlign_VertCenter, 120);
   this.ExposureNumber_Edit = new editBox(this, "", false, FrameStyle_Box, 50);
   this.ExposureNumber_Edit.setFixedWidth(80);
   ttStr = "Number of exposures."
   this.ExposureNumber_Edit.toolTip = ttStr;
   this.ExposureNumber_Edit.onTextUpdated = function(text)
   {
      numberOfExposures=parseInt(text);
   }



   ttStr = "Start exposure."
   this.StartExposure_PushButton = new pushButton(this, "Start exposure", "", ttStr);
   this.StartExposure_PushButton.onClick = function()
   {
      if (expDuration!="" && numberOfExposures>0 ){

         var starSizeMeans=[];
         var numberOfStars=[];
         var imgCount=1397;
         for (var i=0; i< numberOfExposures; i++){
            Console.write("Starting exposure #");Console.writeln(i);
            indi.sendNewProperty("/" + currentCamera + "/CCD_EXPOSURE/CCD_EXPOSURE_VALUE","INDI_NUMBER",expDuration);
            // open image
            //var imgWindow=ImageWindow.open("C:/Users/klaus/tmp/2013_04_07_"+imgCount+".CR2");
            var imgWindow=ImageWindow.open("C:/Users/klaus/tmp/Image.fits");
            imgCount=imgCount+1;
            imgWindow[0].show();

            if (!ImageWindow.activeWindow.isNull && globMeasureStarSizes )
            {
               var D = new StarDetector;
               D.sensitivity=globStarDetectionSensitivity;
               var starArray = D.stars(ImageWindow.activeWindow.mainView.image);
               var maxStar=max(starArray);
               starSizeMeans.push([maxStar[0],maxStar[1],starArray.length]);
               dumpStarSizes(starSizeMeans);
               indi.startTimer(10);
               while (timerIsBusy){console.flush();processEvents();}
               indi.stopTimer();
            }
         }
      }
   }

   this.Exposure_HSizer = new HorizontalSizer;
   with(this.Exposure_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.ExposureDuration_Label);
      add(this.ExposureDuration_Edit);
      add(this.ExposureNumber_Label);
      add(this.ExposureNumber_Edit);
      addStretch();
      add(this.StartExposure_PushButton);

   }

   this.MeasureStarSizes_Label =new labelBox(this, "Measure star sizes:", TextAlign_VertCenter, 120);
   this.MeasureStarSizes_CheckBox = new CheckBox(this);
   this.MeasureStarSizes_CheckBox.onCheck = function()
   {
      if (this.checked)
         globMeasureStarSizes=true;
      else
         globMeasureStarSizes=false;
   }

   this.DetectionSensitivity_Label = new labelBox(this, "Sensitivity:", TextAlign_VertCenter, 120);
   this.DetectionSensitivity_Edit = new editBox(this, "50", false, FrameStyle_Box, 50);
   this.DetectionSensitivity_Edit.setFixedWidth(80);
   ttStr = "Star detection sensitivity. The higher the less stars are detected."
   this.DetectionSensitivity_Edit.toolTip = ttStr;
   this.DetectionSensitivity_Edit.onTextUpdated = function(text)
   {
      globStarDetectionSensitivity=parseInt(text);
   }


   this.MeasureStarSizes_HSizer = new HorizontalSizer;
   with(this.MeasureStarSizes_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.MeasureStarSizes_Label);
      add(this.MeasureStarSizes_CheckBox);
      add(this.DetectionSensitivity_Label);
      add(this.DetectionSensitivity_Edit);
      addStretch();

   }

   // End of frame aquisition ========================================================


   // BEGIN of plate solving:  ========================================================

   this.curRA_Label = new labelBox(this, "Telescope RA:", TextAlign_VertCenter, 120);

   var curRA=[0,0,0];
   this.curRA_Edit = new editBox(this,curRA[0]+":" +curRA[1]+":"+curRA[2], true, FrameStyle_Box, 50);
   this.curRA_Edit.setFixedWidth(80);

   this.curDEC_Label = new labelBox(this, "DEC:", TextAlign_VertCenter, 120);

   var curDEC=[0,0,0];
   this.curDEC_Edit = new editBox(this,curDEC[0]+":" +curDEC[1]+":"+curDEC[2], true, FrameStyle_Box, 50);
   this.curDEC_Edit.setFixedWidth(80);

   this.solver = new ImageSolver();

   this.configSolver_Button = new PushButton(this);
   this.configSolver_Button.text = "Configure solver";
   this.configSolver_Button.toolTip = "<p>Opens the configuration dialog for the script ImageSolver</p>";
   this.configSolver_Button.onClick = function ()
   {


      do {
         var solverWindow = null;
         solverWindow = ImageWindow.activeWindow;

         if (solverWindow == null)
            return new MessageBox("There is not any selected file or window", TITLE, StdIcon_Error, StdButton_Ok).execute();
         this.dialog.solver.Init(solverWindow);
         var dialog = new ImageSolverDialog(this.dialog.solver.solverCfg, this.dialog.solver.metadata);
         var res = dialog.execute();
         if (!res)
         {
            if (dialog.resetRequest)
               this.dialog.solver = new ImageSolver();
            else
               return null;
         }
         this.dialog.solver.solverCfg.SaveSettings();
         this.dialog.solver.metadata.SaveSettings();
      } while (!res);
      return null;
   };

   this.goto_Button = new PushButton(this);
   this.goto_Button.text = "Goto";
   this.goto_Button.toolTip = "<p>Goto position as defined in the solver configuration</p>";
   this.goto_Button.onClick = function (){
     var raInHours=this.dialog.solver.metadata.ra*24/360;
     var decInHours=this.dialog.solver.metadata.dec;
     var propertyArray=[["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA","INDI_NUMBER",raInHours.toString()],
                        ["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC","INDI_NUMBER",decInHours.toString()]];

     periodicTimer.start();
     indi.sendNewPropertyArray(propertyArray);
     periodicTimer.stop();

   }

   this.ImageSolver_HSizer = new HorizontalSizer;
   with(this.ImageSolver_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.curRA_Label);
      add(this.curRA_Edit);
      add(this.curDEC_Label);
      add(this.curDEC_Edit);
      add(this.configSolver_Button);
      addStretch();
      add(this.goto_Button);
   }

   this.ImgRA_Label = new labelBox(this, "Image       RA:", TextAlign_VertCenter, 120);

   var ImgRA=[0,0,0];
   this.ImgRA_Edit = new editBox(this,ImgRA[0]+":" +ImgRA[1]+":"+ImgRA[2], true, FrameStyle_Box, 50);
   this.ImgRA_Edit.setFixedWidth(80);

   this.ImgDEC_Label = new labelBox(this, "DEC:", TextAlign_VertCenter, 120);

   var ImgDEC=[0,0,0];
   this.ImgDEC_Edit = new editBox(this,ImgDEC[0]+":" +ImgDEC[1]+":"+ImgDEC[2], true, FrameStyle_Box, 50);
   this.ImgDEC_Edit.setFixedWidth(80);

   this.solve_Button = new PushButton(this);
   this.solve_Button.text = "Solve";
   this.solve_Button.toolTip = "<p>Start plate solving for current image.</p>";
   this.solve_Button.onClick = function (){

      this.dialog.solver.SolveImage();

      var raInHours=this.dialog.solver.metadata.ra*24/360;
      var RA=convertToHMS(raInHours);

      this.dialog.ImgRA_Edit.text=RA[0]+":" +RA[1]+":"+RA[2];

      var decInHours=this.dialog.solver.metadata.dec;
      var DEC=convertToHMS(decInHours);

      this.dialog.ImgDEC_Edit.text=DEC[0]+":" +DEC[1]+":"+DEC[2];
   }

   this.synch_Button = new PushButton(this);
   this.synch_Button.text = "Synch";
   this.synch_Button.toolTip = "<p>Synchronize telescope coordinates with image coordinates.</p>";
   this.synch_Button.onClick = function (){

     var curRa =parseFloat(indi.getPropertyValue("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA"));
     var curDec =parseFloat(indi.getPropertyValue("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC"));
     var raCor=2*curRa - this.dialog.solver.metadata.ra*24/360;
     var decCor=2*curDec -this.dialog.solver.metadata.dec;
     var propertyArray=[["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA","INDI_NUMBER",raCor.toString()],
                        ["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC","INDI_NUMBER",decCor.toString()]];

     indi.sendNewPropertyArray(propertyArray);
     this.dialog.curRA_Edit.update();
   }


   this.ImageSolver_HSizer2 = new HorizontalSizer;
   with(this.ImageSolver_HSizer2)
   {
      margin = 4;
      spacing = 6;
      add(this.ImgRA_Label);
      add(this.ImgRA_Edit);
      add(this.ImgDEC_Label);
      add(this.ImgDEC_Edit);
      addStretch();
      add(this.solve_Button);
      add(this.synch_Button);
   }



   // END of plate solving:  ========================================================

   // Begin of INDI device properties:  ======================================================
   this.INDIProperties_TreeBox = new TreeBox(this);
   this.PropertyTreeRootNode=null;
   with(this.INDIProperties_TreeBox)
   {
      alternateRowColor = true;
      multipleSelection = false;
      numberOfColumns = 3;
      showColumn(2, false);
      setMinSize(500, 200);
      setColumnWidth(0, 400);
      setColumnWidth(1, 50);
      setColumnWidth(2, 50);
   }
   this.INDIProperties_TreeBox.onNodeSelectionUpdated = function()
   {
      var selected = this.selectedNodes;
      selectedPropertyNode = selected[0];
   }
   ttStr = "Show INDI device properties.";
   this.showProperties_PushButton = new pushButton(this, "Properties", "", ttStr);
   this.showProperties_PushButton.onClick = function()
   {
      this.dialog.PropertyTreeRootNode = createPropertyTree(this.dialog.INDIProperties_TreeBox);
   }

   ttStr = "Set new property value.";
   this.setPropertyValue_PushButton = new pushButton(this, "Set property", "", ttStr);
   this.setPropertyValue_PushButton.onClick = function()
   {
         var propertyType=selectedPropertyNode.text(2);
         var propertyValue=selectedPropertyNode.text(1);
         var propertyString= "/" + selectedPropertyNode.parent.parent.text(0) + "/" + selectedPropertyNode.parent.text(0) + "/" + selectedPropertyNode.text(0);
         var setPropDialog = new SetPropertyDialog(propertyString,propertyValue,propertyType);
         if (setPropDialog.execute())
         {
            var visitorObj = new visitor();
            var newValue = indi.getPropertyValue(propertyString);
            this.dialog.PropertyTreeRootNode.accept(visitorObj,propertyString,newValue);
         }
         else {
             Console.writeln("Property dialog could not be executed");
         }


   }

   this.INDIProperties_HSizer = new HorizontalSizer;
   this.INDIPropertiesButtons_VSizer = new VerticalSizer;
   with(this.INDIPropertiesButtons_VSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.showProperties_PushButton);
      add(this.setPropertyValue_PushButton);
      addStretch();
   }
   with(this.INDIProperties_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDIProperties_TreeBox);
      add(this.INDIPropertiesButtons_VSizer);

      addStretch();
   }
   // End of INDI device properties:  ========================================================


   this.sizer = new VerticalSizer;
   with(this.sizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDIServer_HSizer);
      add(this.INDICamera_HSizer);
      add(this.INDITelescope_HSizer);
      add(this.Exposure_HSizer);
      add(this.MeasureStarSizes_HSizer);
      add(this.ImageSolver_HSizer);
      add(this.ImageSolver_HSizer2);
      add(this.INDIProperties_HSizer);
   }
}

mainDialog.prototype.startTimer = function(interval){
   this.timer.interval=interval;
   this.timer.start();
   timerIsBusy=true;
}

mainDialog.prototype.stopTimer = function(interval){
   this.timer.stop();
   timerIsBusy=false;
}



ComboBox.prototype.update = function(){
   this.dialog.INDICamera_Combo.clear();
   this.dialog.INDITelescope_Combo.clear();
   var deviceList={};
   console.writeln(indi.INDI.INDI_Properties.length);
   for (var i=0; i<indi.INDI.INDI_Properties.length; ++i){
      if (indi.INDI.INDI_Properties[i][0]!="")
      {
         var splittedString = indi.INDI.INDI_Properties[i][0].split("/");
         var deviceString = splittedString[1];
         if (deviceString in deviceList){
            continue;
         }
         else {
            deviceList[deviceString]=true;
            this.dialog.INDICamera_Combo.addItem(deviceString);
            this.dialog.INDITelescope_Combo.addItem(deviceString);
         }
      }
   }

   // set default telescope
   currentTelescope=this.dialog.INDITelescope_Combo.itemText(this.dialog.INDITelescope_Combo.currentItem);
   // set default camera
   currentCamera=this.dialog.INDICamera_Combo.itemText(this.dialog.INDICamera_Combo.currentItem);

}

Edit.prototype.update = function(){
   var curRA=[0,0,0];
   if (indi.CheckPropertyExists("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA")){
      curRA=convertToHMS(parseFloat(indi.getPropertyValue("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA")));
   }
   this.dialog.curRA_Edit.text=curRA[0]+":" +curRA[1]+":"+curRA[2];

   var curDEC=[0,0,0];
   if (indi.CheckPropertyExists("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC")){
     curDEC=convertToHMS(parseFloat(indi.getPropertyValue("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC")));
   }
   this.dialog.curDEC_Edit.text=curDEC[0]+":" +curDEC[1]+":"+curDEC[2];
}

mainDialog.prototype = new Dialog;
var maindlg = new mainDialog();


function onUpdateTimerEvent (){
 switch (maindlg.INDIAsynchAction)
 {
 case maindlg.INDIAsynchActionEnum.ServerConnect:
      maindlg.INDICamera_Combo.update();
      break;
 case maindlg.INDIAsynchActionEnum.TelescopeConnect:
      maindlg.curRA_Edit.update();
      break;
 default:
 }
}

function onUpdatePeriodicTimerEvent (){
 maindlg.curRA_Edit.update();
}

var timer = new Timer();
timer.singleShot=true;
timer.periodic=false;
timer.interval=2;
timer.onTimeout=onUpdateTimerEvent;

var periodicTimer = new Timer();
periodicTimer.singleShot=false;
periodicTimer.periodic=true;
periodicTimer.interval=1;
periodicTimer.onTimeout=onUpdatePeriodicTimerEvent;



var globNewProperty="";
var globNewPropertyValue="";
var globNewPropertyType="";
var globMeasureStarSizes=false;
var globStarDetectionSensitivity=50;
var currentCamera="";
var currentTelescope="";

var expDuration = "";
var numberOfExposures;
function main()
{
   //console.hide();
   // Show our dialog box, quit if cancelled.

   if (maindlg.execute())
   {

   }

   //console.hide();
}

main();
