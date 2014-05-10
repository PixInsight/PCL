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


   var ttStr = "";



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


   this.INDIProcess_Combo.onItemSelected = function ()
   {
      if (indi.INDI!=null)
        indi.INDI.readIcon(this.dialog.INDIProcess_Combo.itemText(this.dialog.INDIProcess_Combo.currentItem));

      this.dialog.INDITelescope_Combo.update();

   }

   this.INDIProcess_HSizer = new HorizontalSizer;
   with(this.INDIProcess_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDIProcess_Label);
      add(this.INDIProcess_Combo);

      addStretch();

   }
   // End of INDI server connection:  ========================================================




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

   this.INDITelescope_HSizer = new HorizontalSizer;
   with(this.INDITelescope_HSizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDITelescope_Label);
      add(this.INDITelescope_Combo);
      addStretch();


   }

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

  //   periodicTimer.start();
     console.writeln("sendNewProperty");
     indi.sendNewPropertyArray(propertyArray);
     console.writeln("...done")
     this.dialog.curRA_Edit.update();
 //    periodicTimer.stop();

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

     var curRa =parseFloat(indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/RA"));
     var curDec =parseFloat(indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC"));
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

   this.sizer = new VerticalSizer;
   with(this.sizer)
   {
      margin = 4;
      spacing = 6;
      add(this.INDIProcess_HSizer);

      add(this.INDITelescope_HSizer);

      add(this.ImageSolver_HSizer);
      add(this.ImageSolver_HSizer2);

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
   if (propRAValue!=""){
      curRA=convertToHMS(parseFloat(propRAValue));
   }
   this.dialog.curRA_Edit.text=curRA[0]+":" +curRA[1]+":"+curRA[2];

   var curDEC=[0,0,0];
   var propDECValue=indi.getPropertyValue2("/" + currentTelescope + "/EQUATORIAL_EOD_COORD/DEC")
   if (propDECValue!=""){
     curDEC=convertToHMS(parseFloat(propDECValue));
   }
   this.dialog.curDEC_Edit.text=curDEC[0]+":" +curDEC[1]+":"+curDEC[2];
}

mainDialog.prototype = new Dialog;
var maindlg = new mainDialog();




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
