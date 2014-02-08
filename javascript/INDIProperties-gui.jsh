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

function SetPropertyDialog(propertyText,propertyValueText) {
   this.__base__ = Dialog;
   this.__base__();

   var ttStr = "";

   globNewProperty=propertyText;
   globNewPropertyValue=propertyValueText;
   globNewPropertyType="INDI_SWITCH";

   this.Property_Label = new labelBox(this, propertyText, TextAlign_VertCenter, 150);

   this.PropertyValue_Edit = new editBox(this, propertyValueText, false, FrameStyle_Box, 50);
   this.PropertyValue_Edit.setFixedWidth(80);
   ttStr = "INDI property value."
   this.PropertyValue_Edit.toolTip = ttStr;
   this.PropertyValue_Edit.onTextUpdated = function(text)
   {
      globNewPropertyValue=text;

   }

   this.PropertyType_Edit = new editBox(this, "INDI_SWITCH", false, FrameStyle_Box, 50);
   this.PropertyType_Edit.setFixedWidth(80);
   ttStr = "INDI property type."
   this.PropertyType_Edit.toolTip = ttStr;
   this.PropertyType_Edit.onTextUpdated = function(text)
   {
      globNewPropertyType=text;
   }

   this.Prop_Hsizer = new HorizontalSizer;
   with(this.Prop_Hsizer)
   {
      margin = 4;
      spacing = 6;
      add(this.Property_Label);
      add(this.PropertyValue_Edit);
      add(this.PropertyType_Edit);
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
