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

#feature-id INDI > INDI Telescope

#feature-info INDI Telescope Control. \
   <br><br/>\
   <br/>\
   Copyright &copy; 2014 Klaus Kretzschmar

#feature-icon INDISynch.xpm

var indi = new INDIclient();

function convertToHMS(floatNum){
   var H=Math.trunc(floatNum);
   var M=Math.trunc((floatNum-H)*60);
   var S=Math.trunc(((floatNum-H)*60 - M)*60);
   return [H,M,S];
}

function convertToCoordInHours(HMS){
	var CoordInHours=parseFloat(HMS[0]) + parseFloat(HMS[1])/60 + parseFloat(HMS[2])/3600;
}

SetPropertyDialog.prototype = new Dialog;

function mainDialog()
{
   this.__base__ = Dialog;
   this.__base__();


   var ttStr = "";

    // BEGIN of INDI Client Process  ========================================================


   this.INDIClientProcess_Section       = new SectionBar(this,"INDI Client Process");
   this.INDIClientProcess_Control       = new Control(this);
   this.INDIClientProcess_Control.sizer = new VerticalSizer;
   this.INDIClientProcess_Control.sizer.spacing = 4;

   this.INDIClientProcess_Section.setSection(this.INDIClientProcess_Control);


   this.INDIProcess_Label = new labelBox(this, "INDI Client Process", TextAlign_VertCenter, 120);
   this.INDIProcess_Label.setFixedWidth(100);
   this.INDIProcess_Combo = new ComboBox(this);
   this.INDIProcess_Combo.editEnabled = false;
   this.INDIProcess_Combo.setMinWidth(150);
   var ttStr = "Choose INDI Process Icon";
   this.INDIProcess_Combo.toolTip = ttStr;

   var iconsArray=ProcessInstance.icons();
   for (var i=0; i<iconsArray.length;++i){
      this.INDIProcess_Combo.addItem(iconsArray[i]);
   }

   if (!indi.INDI)
      indi.INDI = new INDIClient;

   indi.registerInstance();


   this.INDIProcess_Combo.onItemSelected = function ()
   {
      if (indi.INDI!=null)
        indi.INDI.readIcon(this.dialog.INDIProcess_Combo.itemText(this.dialog.INDIProcess_Combo.currentItem));

      this.dialog.INDITelescope_Combo.update();

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
      this.dialog.curRA_Edit.update();
   }

   this.INDIProcess_HSizer = new HorizontalSizer;
   with(this.INDIProcess_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDIProcess_Label);
      add(this.INDIProcess_Combo);
      addStretch();
      add(this.INDITelescope_Label);
      add(this.INDITelescope_Combo);
      addStretch();

   }
   this.INDIClientProcess_Control.sizer.add(this.INDIProcess_HSizer);

   // BEGIN of Telescope Position Section:  ===========================================
   this.TelescopePosition_Section       = new SectionBar(this,"Telescope Position");
   this.TelescopePosition_Control       = new Control(this);
   this.TelescopePosition_Control.sizer = new VerticalSizer;
   this.TelescopePosition_Control.sizer.spacing = 4;

   this.TelescopePosition_Section.setSection(this.TelescopePosition_Control);


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
   this.configSolver_Button.text = "Choose Object";
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

     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     this.dialog.curRA_Edit.update();
     //periodicTimer.start();

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


   this.TelescopePosition_Control.sizer.add(this.ImageSolver_HSizer);

   this.Motion_Label = new labelBox(this, "Telescope Motion:", TextAlign_VertCenter, 120);

   this.moveNorth_Button = new PushButton(this);
   this.moveNorth_Button.text = "North";
   this.moveNorth_Button.toolTip = "<p>Move telescope towards North</p>";
   this.moveNorth_Button.onPress = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_NS/MOTION_NORTH","INDI_SWITCH","ON"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     //periodicTimer.start();
   }

   this.moveNorth_Button.onRelease = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_NS/MOTION_NORTH","INDI_SWITCH","OFF"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     this.dialog.curDEC_Edit.update();
     //periodicTimer.start();
   }

   this.MotionControl_HSizer = new HorizontalSizer;
   with(this.MotionControl_HSizer)
   {
      margin = 4;
      spacing = 6;
      addStretch();
      add(this.moveNorth_Button);
   }

   this.moveSouth_Button = new PushButton(this);
   this.moveSouth_Button.text = "South";
   this.moveSouth_Button.toolTip = "<p>Move telescope towards South</p>";
   this.moveSouth_Button.onPress = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_NS/MOTION_SOUTH","INDI_SWITCH","ON"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     //periodicTimer.start();
   }

   this.moveSouth_Button.onRelease = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_NS/MOTION_SOUTH","INDI_SWITCH","OFF"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     this.dialog.curDEC_Edit.update();
     //periodicTimer.start();
   }

   this.moveWest_Button = new PushButton(this);
   this.moveWest_Button.text = "West";
   this.moveWest_Button.toolTip = "<p>Move telescope towards West</p>";
   this.moveWest_Button.onPress = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_WE/MOTION_WEST","INDI_SWITCH","ON"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     //periodicTimer.start();
   }

   this.moveWest_Button.onRelease = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_WE/MOTION_WEST","INDI_SWITCH","OFF"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
      this.dialog.curRA_Edit.update();
      //periodicTimer.start();
   }

   this.moveEast_Button = new PushButton(this);
   this.moveEast_Button.text = "East";
   this.moveEast_Button.toolTip = "<p>Move telescope towards East</p>";
   this.moveEast_Button.onPress = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_WE/MOTION_EAST","INDI_SWITCH","ON"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     //periodicTimer.start();
   }

   this.moveEast_Button.onRelease = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_MOTION_WE/MOTION_EAST","INDI_SWITCH","OFF"]]
     periodicTimer.stop();
     indi.sendNewPropertyArrayAsynch(propertyArray);
     this.dialog.curRA_Edit.update();
     //periodicTimer.start();
   }

   this.MotionControlNS_VSizer = new VerticalSizer;
   with( this.MotionControlNS_VSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.moveNorth_Button);
      addStretch();
      add(this.moveSouth_Button);
   }

   this.MotionControlW_VSizer = new VerticalSizer;
   with( this.MotionControlW_VSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.moveWest_Button);
   }

   this.MotionControlE_VSizer = new VerticalSizer;
   with( this.MotionControlE_VSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.moveEast_Button);
   }

   this.MotionControl_HSizer = new HorizontalSizer;
   with(this.MotionControl_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.Motion_Label);
      addStretch();
      add(this.MotionControlW_VSizer);
      add(this.MotionControlNS_VSizer);
      add(this.MotionControlE_VSizer);
      addStretch();
   }


   this.TelescopePosition_Control.sizer.add(this.MotionControl_HSizer);

   this.Park_Label = new labelBox(this, "Parking Telescope:", TextAlign_VertCenter, 120);

   this.Park_Button = new PushButton(this);
   this.Park_Button.text = "Park";
   this.Park_Button.toolTip = "<p>Move telescope towards the parking position.</p>";
   this.Park_Button.onPress = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_PARK/PARK","INDI_SWITCH","ON"]]
     periodicTimer.stop();
     indi.sendNewPropertyArray(propertyArray);
     //periodicTimer.start();
   }

   this.Unpark_Button = new PushButton(this);
   this.Unpark_Button.text = "Unpark";
   this.Unpark_Button.toolTip = "<p>Unpark telescope.</p>";
   this.Unpark_Button.onPress = function (){
     var propertyArray=[["/" + currentTelescope + "/TELESCOPE_PARK/PARK","INDI_SWITCH","ON"]]
     periodicTimer.stop();
     indi.sendNewPropertyArray(propertyArray);
     //periodicTimer.start();
   }

   this.Park_HSizer = new HorizontalSizer;
   with(this.Park_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.Park_Label);
      add(this.Unpark_Button);
      add(this.Park_Button);
      addStretch();
   }

   this.TelescopePosition_Control.sizer.add(this.Park_HSizer);

   // BEGIN of Image Centralization Section:  ===========================================
   this.ImageCentralization_Section       = new SectionBar(this,"Image Centralization");
   this.ImageCentralization_Control       = new Control(this);
   this.ImageCentralization_Control.sizer = new VerticalSizer;
   this.ImageCentralization_Control.sizer.spacing = 4;

   this.ImageCentralization_Section.setSection(this.ImageCentralization_Control);

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
      periodicTimer.stop();
      this.dialog.solver.SolveImage();

      var raInHours=this.dialog.solver.metadata.ra*24/360;
      var RA=convertToHMS(raInHours);

      this.dialog.ImgRA_Edit.text=RA[0]+":" +RA[1]+":"+RA[2];

      var decInHours=this.dialog.solver.metadata.dec;
      var DEC=convertToHMS(decInHours);

      this.dialog.ImgDEC_Edit.text=DEC[0]+":" +DEC[1]+":"+DEC[2];
      //periodicTimer.start();
   }

   this.center_Button = new PushButton(this);
   this.center_Button.text = "Center";
   this.center_Button.toolTip = "<p>Center the object with based on the given image coordinates.</p>";
   this.center_Button.onClick = function (){
	 
     var curRa =parseFloat(convertToCoordInHours(indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA").split(":")));
     var curDec =parseFloat(convertToCoordInHours(indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC").split(":")));
     var raCor=2*curRa - this.dialog.solver.metadata.ra*24/360;
     var decCor=2*curDec -this.dialog.solver.metadata.dec;
     var propertyArray=[["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA","INDI_NUMBER",raCor.toString()],
                        ["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC","INDI_NUMBER",decCor.toString()]];

     periodicTimer.stop();
     indi.sendNewPropertyArray(propertyArray);
     this.dialog.curRA_Edit.update();
     //periodicTimer.start();
   }

   this.synch_Button = new PushButton(this);
   this.synch_Button.text = "Sync";
   this.synch_Button.toolTip = "<p>Synchronize the image coordinates with the motor positions.</p>";
   this.synch_Button.onClick = function (){

     periodicTimer.stop();
     var propertyArray=[["/" + currentTelescope + "/ON_COORD_SET/SYNC","INDI_SWITCH","ON"]]
     indi.sendNewPropertyArray(propertyArray);

     var curRa =parseFloat(convertToCoordInHours(indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA").split(":")));
     var curDec =parseFloat(convertToCoordInHours(indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC").split(":")));
     propertyArray=[["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA","INDI_NUMBER",curRa.toString()],
                    ["/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC","INDI_NUMBER",curDec.toString()]];

     indi.sendNewPropertyArray(propertyArray);
     this.dialog.curRA_Edit.update();

     var propertyArray=[["/" + currentTelescope + "/ON_COORD_SET/TRACK","INDI_SWITCH","ON"]]
     indi.sendNewPropertyArray(propertyArray);
     //periodicTimer.start();
   }

   this.ImageSolver_HSizer = new HorizontalSizer;
   with(this.ImageSolver_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.ImgRA_Label);
      add(this.ImgRA_Edit);
      add(this.ImgDEC_Label);
      add(this.ImgDEC_Edit);
      addStretch();
      add(this.solve_Button);
      add(this.center_Button);
      add(this.synch_Button);
   }

   this.ImageCentralization_Control.sizer.add(this.ImageSolver_HSizer);


   // END of plate solving:  ========================================================

   this.sizer = new VerticalSizer;
   with(this.sizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDIClientProcess_Section);
      add(this.INDIClientProcess_Control);

      add(this.TelescopePosition_Section);
      add(this.TelescopePosition_Control);

      add(this.ImageCentralization_Section);
      add(this.ImageCentralization_Control);

   }
}

ComboBox.prototype.update = function(){
   this.dialog.INDITelescope_Combo.clear();
   if (indi.INDI!=null){

      for (var i=0; i<indi.INDI.INDI_Devices.length; ++i){
         this.dialog.INDITelescope_Combo.addItem(String(indi.INDI.INDI_Devices[i]));
      }

      // set default telescope
      currentTelescope=this.dialog.INDITelescope_Combo.itemText(this.dialog.INDITelescope_Combo.currentItem);

   }
}

Edit.prototype.update = function(){
   var curRA=[0,0,0];
   var propRAValue=indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA")
  
   this.dialog.curRA_Edit.text=propRAValue;

   var curDEC=[0,0,0];
   var propDECValue=indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC")
   
   this.dialog.curDEC_Edit.text=propDECValue;
}

mainDialog.prototype = new Dialog;
var maindlg = new mainDialog();


maindlg.onClose = function () {
  indi.releaseInstance();
}


function onUpdatePeriodicTimerEvent (){
 maindlg.curRA_Edit.update();
}


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
var currentTelescope="";

var expDuration = "";
var numberOfExposures;
function main()
{
   //console.hide();
   // Show our dialog box, quit if cancelled.
   console.abortEnabed=true;

   if (maindlg.execute())
   {

   }

   //console.hide();
}

main();
