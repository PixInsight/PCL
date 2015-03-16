//push button object constructor
function pushButton(parent, bText, bIcon, bToolTip)
{
   this.button = new PushButton(parent);
   if(bText != '')
      this.button.text = bText;
   if(bIcon != '')
      this.button.icon = new Bitmap(bIcon);
   if(bToolTip != '')
      this.button.toolTip = bToolTip;

   return this.button;
}

//label object constructor
function labelBox(parent, elText, tAlign, elWidth)
{
   this.label = new Label(parent);
   this.label.text = elText;
   this.label.textAlignment = tAlign;
   this.label.setMaxWidth(elWidth);

   return this.label;
}

//edit box object constructor
function editBox(parent, eText, readWrite, eStyle, eMinWidth)
{
   this.edit = new Edit(parent);
   if(eText != '')
      this.edit.text = eText;
   this.edit.readOnly = readWrite;
   this.edit.style = eStyle;
   this.edit.setMinWidth(eMinWidth);

   return this.edit;
}

//spin box object constructor
function spinBox(parent, sbValue, sbMinVal, sbMaxVal, sbStep, sbCanEdit)
{
   this.spinbox = new SpinBox(parent);
   this.spinbox.value = sbValue;
   this.spinbox.setRange(sbMinVal, sbMaxVal);
   this.spinbox.stepSize = sbStep;
   this.spinbox.editable = sbCanEdit;

   return this.spinbox;
}



function SetPropertyDialog(propertyText,propertyValueText, propertyTypeText) {
   this.__base__ = Dialog;
   this.__base__();

   var ttStr = "";

   globNewProperty=propertyText;
   globNewPropertyValue=propertyValueText;
   globNewPropertyType=propertyTypeText;

   this.Property_Label = new labelBox(this, propertyText, TextAlign_VertCenter, 500);

   this.PropertyValue_Edit = new editBox(this, propertyValueText, false, FrameStyle_Box, 50);
   this.PropertyValue_Edit.setFixedWidth(80);
   ttStr = "INDI property value."
   this.PropertyValue_Edit.toolTip = ttStr;
   this.PropertyValue_Edit.onTextUpdated = function(text)
   {
      globNewPropertyValue=text;

   }


   this.Prop_Hsizer = new HorizontalSizer;
   with(this.Prop_Hsizer)
   {
      margin = 4;
      spacing = 6;
      add(this.Property_Label);
      add(this.PropertyValue_Edit);
   }

   this.ok_Button = new PushButton( this );
   this.ok_Button.text = "OK";
   this.ok_Button.icon = ":/icons/ok.png";
   this.ok_Button.cursor = new Cursor( StdCursor_Checkmark );
   this.ok_Button.onClick = function()
   {
      indi.sendNewProperty(globNewProperty,globNewPropertyType,globNewPropertyValue);
      this.dialog.ok();

   };

   this.cancel_Button = new PushButton( this );
   this.cancel_Button.text = "Cancel";
   this.cancel_Button.icon = ":/icons/cancel.png";
   this.cancel_Button.cursor = new Cursor( StdCursor_Crossmark );
   this.cancel_Button.onClick = function()
   {
     this.dialog.cancel();
   };

   this.OK_Hsizer = new HorizontalSizer;
   with(this.OK_Hsizer)
   {
      margin = 4;
      spacing = 6;
      add(this.ok_Button);
      add(this.cancel_Button);

   }

   this.sizer = new VerticalSizer;
   with(this.sizer)
   {
      margin = 4;
      spacing = 6;
      add(this.Prop_Hsizer);
      add(this.OK_Hsizer);

   }

}

Object.prototype.getName = function() {
   var funcNameRegex = /function (.{1,})\(/;
   var results = (funcNameRegex).exec((this).constructor.toString());
   return (results && results.length > 1) ? results[1] : "";
};




function PropertyNode(parent, property){

   this.child=[];
   this.numberOfChildren=0;
   //this.parentNode=parent;
   this.propertyStr=property;
   this.treeBoxNode=null;
   if (parent!=null){
      parent.child.push(this);
      parent.numberOfChildren++;
      this.treeBoxNode=new TreeBoxNode(parent.treeBoxNode);
   }

   this.accept = function(visitorObj,property,value){
      visitorObj.visit(this,property,value);
      for (var i=0; i<this.numberOfChildren; ++i){
         this.child[i].accept(visitorObj, property,value);
      }
   }

  this.clear = function(){
     if (this.treeBoxNode!=null){
      this.treeBoxNode.clear();
     }
  }

  this.setTreeBoxNode = function(treeBoxNode){
      this.treeBoxNode=treeBoxNode;
  }
}

function visitor(){
   this.visit = function(propertyNode,property,value){
      if (propertyNode.propertyStr===property){
         var splittedString = propertyNode.propertyStr.split("/");
         propertyNode.treeBoxNode.setText(0,splittedString[3]);
         propertyNode.treeBoxNode.setText(1,value);
      }
   }
}

function createPropertyTree(node){
  var rootNode=new PropertyNode(null,"");
  rootNode.setTreeBoxNode(node);
  rootNode.treeBoxNode.clear();
  var deviceNodeMap={};
  var propertyNodeMap={};
  for (var i=0; i<indi.INDI.INDI_Properties.length; ++i){
    if (indi.INDI.INDI_Properties[i][0]!="")
    {
       var splittedString = indi.INDI.INDI_Properties[i][0].split("/");

       // device nodes
       var deviceNode;
       var deviceString = splittedString[1];
       if ( deviceString in deviceNodeMap)
       {
         deviceNode = deviceNodeMap[deviceString];
       }
       else
       {
         var childNode = new PropertyNode(rootNode,"/"+deviceString);
         childNode.treeBoxNode.setText(0,splittedString[1]);
         deviceNodeMap[deviceString]=childNode;
         deviceNode=childNode;
       }
       // property nodes
       var propertyNode;
       var propertyString = splittedString[2];
       var keyString = deviceString+propertyString;
       if ( keyString in propertyNodeMap)
       {
         propertyNode = propertyNodeMap[keyString];
       }
       else
       {
         var childNode = new PropertyNode(deviceNode,"/" +deviceString +"/" +propertyString);
         childNode.treeBoxNode.setText(0,splittedString[2]);
         propertyNodeMap[keyString]=childNode;
         propertyNode=childNode;
       }
       // property value nodes
       var childNode = new PropertyNode(propertyNode,"/" +deviceString +"/" +propertyString+"/"+splittedString[3]);
       childNode.treeBoxNode.setText(0,splittedString[3]);
       childNode.treeBoxNode.setText(1,indi.INDI.INDI_Properties[i][1]);
       childNode.treeBoxNode.setText(2,indi.INDI.INDI_Properties[i][3]);
     }
    }
    return rootNode;
}


