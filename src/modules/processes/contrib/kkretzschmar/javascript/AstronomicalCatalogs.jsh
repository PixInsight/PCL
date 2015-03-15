/*
   Astronomical Catalogs

   This file is part of ImageSolver and AnnotateImage scripts

   Copyright (C) 2012, Andres del Pozo
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define NULLMAG 1000

function CatalogRegister()
{
   this.catalogs = new Array();

   this.Register = function ( catalog )
   {
      this.catalogs.push( {name:catalog.name, constructor:catalog.GetConstructor() } );
   }

   this.FindByName = function ( catalogName )
   {
      for( var i=0; i<this.catalogs.length; i++)
         if( this.catalogs[i].name == catalogName )
            return this.catalogs[i];
      return null;
   }

   this.GetCatalog = function( idx )
   {
      if(typeof(idx)=="string")
      {
         var cat = this.FindByName(idx);
         if( cat==null )
            return null;
         else
            return eval( cat.constructor );
      } else
         return eval( this.catalogs[idx].constructor );
   }
}
var __catalogRegister__ = new CatalogRegister();

// ******************************************************************
// CatalogRecord: Stores the information of a record of a catalog
// ******************************************************************
function CatalogRecord(positionRD, diameter, name, magnitude)
{
   this.posRD=positionRD;  // Position where x=RA(deg) and y=Dec(deg)
   this.diameter=diameter; // Diameter of the object in degrees
   this.name=name;         // Name of the object
   if(magnitude)
      this.magnitude = magnitude;  // Magnitude of the object. Can be undefined
}

// ******************************************************************
// Catalog: Base class for all catalogs
// ******************************************************************
function Catalog( name )
{
   this.__base__ = ObjectWithSettings;
   this.__base__(
      SETTINGS_MODULE,
      name,
      new Array(
         ["visible",DataType_Boolean]
      )
   );

   this.properties = new Array();
   this.objects = null;
}

Catalog.prototype = new ObjectWithSettings;

// ******************************************************************
// NGCICCatalog: Catalog NGC/IC. Uses a file to store the info
// ******************************************************************
function NGCICCatalog()
{
   this.name = "NGC-IC";
   this.description = "NGC and IC catalogs (9900 objects)";

   this.__base__ = Catalog;
   this.__base__( this.name );

   this.fields = [ "Name", "Coordinates", "NGC/IC code" ];
   this.properties.push(["catalogPath", DataType_String]);

   /* ### 1.8.0 RC5 */
   this.setDefaultCatalogPath = function()
   {
      this.catalogPath = File.extractDrive( #__FILE__ ) + File.extractDirectory( #__FILE__ );
      if ( this.catalogPath[this.catalogPath.length-1] != '/' )
         this.catalogPath += '/';
      this.catalogPath += "ngc2000.txt";
   };

   this.GetConstructor = function()
   {
      return "new NGCICCatalog()";
   };

   this.Validate = function()
   {
      if ( !this.catalogPath || this.catalogPath.trim().length == 0 || !File.exists( this.catalogPath ) )
      {
         var badPath = this.catalogPath;
         this.setDefaultCatalogPath();
         if ( badPath && badPath.trim().length > 0 )
            console.writeln( "<end><cbr>** Warning: the NGC/IC catalog file does not exist: " + badPath +
                             "\n** Falling back to the default local catalog file: " + this.catalogPath );
      }
      if ( !File.exists( this.catalogPath ) ) {
         (new MessageBox( "Unable to load NGC/IC catalog file.", TITLE, StdIcon_Error, StdButton_Ok )).execute();
         return false;
      }
      return true;
   };

   this.Load = function( metadata )
   {
      var bounds = metadata.FindImageBounds();

      var file = new File();
      if( !this.catalogPath )
         return false;
      file.openForReading( this.catalogPath );
      if( !file.isOpen )
         return false;

      var s = file.read( DataType_ByteArray,file.size );
      file.close();
      this.catalogLines = s.toString().split("\r\n");

      this.objects = new Array;
      for( var i=1; i<this.catalogLines.length; i++ )
      {
         var fields = this.catalogLines[i].split("\t");
         if(fields.length<8)
            continue;
         var posRD = new Point(parseFloat(fields[3])*15, parseFloat(fields[4]));
         var posI = metadata.Convert_RD_I(posRD);

         if( posI!=null && posI.x>0 && posI.y>0 && posI.x<metadata.width && posI.y<metadata.height)
         //if( x>bounds.left && x<bounds.right && y>bounds.top && y<bounds.bottom )
         {
            var diameter = parseFloat( fields[5] )/60;

            var ngcic;
            if( fields[0]=="I" )
               ngcic="IC"+fields[1];
            else
               ngcic="NGC"+fields[1];

            var name = ngcic;
            if( fields[7]!=null && fields[7].length>0 )
               name+=" / "+fields[7];

            var record = new CatalogRecord( posRD, diameter, name );
            this.objects.push( record );
            record["NGC/IC code"] = ngcic;
         }
      }

      console.writeln( "\n<b>Catalog NGC/IC size</b>: ", this.objects.length, " of ", this.catalogLines.length-1, " objects" );

	  return true;
   };

   this.GetEditControls = function( parent )
   {
      // Catalog path
      var path_Label = new Label( parent );
      path_Label.text = "Catalog path:";
      path_Label.textAlignment = TextAlign_Right|TextAlign_VertCenter;

      var path_Edit = new Edit(parent);
      path_Edit.text = this.catalogPath ? this.catalogPath : "";
      path_Edit.onTextUpdated = function( value ) { this.dialog.activeFrame.object.catalog.catalogPath = value; };

      var path_Button = new ToolButton( parent );
      path_Button.icon = ":/icons/select-file.png";
      path_Button.toolTip = "<p>Select the NGC/IC catalog file.</p>";
      path_Button.onClick = function()
      {
         var gdd = new OpenFileDialog;
         if ( this.dialog.activeFrame.object.catalog.catalogPath )
            gdd.initialPath = this.dialog.activeFrame.object.catalog.catalogPath;
         gdd.caption = "Select NGC/IC Catalog Path";
         gdd.filters = [["Text files", "*.txt"]];
         if ( gdd.execute() )
         {
            this.dialog.activeFrame.object.catalog.catalogPath = gdd.fileName;
            path_Edit.text = gdd.fileName;
         }
      };

      var pathSizer = new HorizontalSizer;
      pathSizer.spacing = 4;
      pathSizer.add(path_Label);
      pathSizer.add(path_Edit,100);
      pathSizer.add(path_Button);

      return [pathSizer];
   };

   this.setDefaultCatalogPath();
}

NGCICCatalog.prototype = new Catalog;
__catalogRegister__.Register( new NGCICCatalog );

// ******************************************************************
// VizierCatalog: Base class for all the catalogs downloaded from
//                VizieR servers
// ******************************************************************
function VizierCatalog( name )
{
   this.__base__ = Catalog;
   this.__base__( name );

   this.UrlBuilder = null;
   this.ParseRecord = null;
   this.catalogMagnitude = null;
   this.magMin = NULLMAG;
   this.magMax = NULLMAG;
   this.epoch = null;

   this.Load = function( metadata, mirrorServer )
   {
      var w = metadata.width/2 * metadata.resolution * Math.cos(metadata.dec);
      var h = metadata.height/2 * metadata.resolution;
      var loadBounds = new Rect(metadata.ra - w/2, metadata.dec - h/2, metadata.ra + w/2, metadata.dec + h/2 );

      // Check if the required area is already loaded
      if( this.bounds &&
          this.bounds.x0<=loadBounds.x0 && this.bounds.y0<=loadBounds.y0 &&
          this.bounds.x1>=loadBounds.x1 && this.bounds.y1>=loadBounds.y1 &&
          this.epoch==metadata.epoch )
      {
         console.writeln( "Catalog ", this.name, " already loaded") ;
         console.writeln( "<b>Catalog ", this.name, " size</b>: ", this.objects.length, " objects" );
         return;
      }


      this.objects = new Array;
      this.bounds = null;
      this.epoch = null;

      var url = this.UrlBuilder( metadata, mirrorServer );

      var outputFileName = File.systemTempDirectory + "/VizierQueryResult.tsv";

      console.writeln( "<end>\n<b>Downloading Vizier data:</b>" );
      console.writeln( "<raw>"+url+"</raw>" );
      var consoleAbort = console.abortEnabled
      console.abortEnabled = true;
      console.show();

      // Send request
      var download = new FileDownload( url, outputFileName );
      try
      {
         download.perform();
      }
      catch ( e )
      {
         (new MessageBox( e.toString(), TITLE, StdIcon_Error, StdButton_Ok )).execute();
      }

      console.abortEnabled = consoleAbort;
      //console.hide();

      if (!download.ok )
         return;

      var file=new File();
      file.openForReading(outputFileName);
      if(!file.isOpen)
         return;
      var s=file.read(DataType_ByteArray,file.size);
      file.close();
      this.catalogLines=s.toString().split("\n");

      var epoch; // Epoch in decimal years (i.e. 2012.456)
      if ( metadata.epoch != null )
      {
         epoch = (metadata.epoch - Math.complexTimeToJD( 2000, 1, 1 )) / 365.25+2000;
         console.writeln( "Using epoch: ", epoch );
      }

      for ( var i=0; i<this.catalogLines.length; i++ )
      {
         var line = this.catalogLines[i];
         if( line.length==0 || line.charAt(0)=="#" ) //comment
            continue;
         var tokens = line.split("|");
         var object = this.ParseRecord( tokens, epoch );
         if ( object && object.posRD.x>=0 && object.posRD.x<=360 && object.posRD.y>=-90 && object.posRD.y<=90 )
         {
            this.objects.push(object);
            if( this.bounds )
               this.bounds = this.bounds.union( object.posRD.x, object.posRD.y, object.posRD.x, object.posRD.y );
            else
               this.bounds = new Rect( object.posRD.x, object.posRD.y, object.posRD.x, object.posRD.y );
         }
         // processEvents();
         // if ( console.abortRequested )
               // throw "Process aborted";
      }
      if ( this.PostProcessObjects )
         this.PostProcessObjects( this.objects, metadata );
      this.epoch = metadata.epoch;

      if( metadata.ref_I_G )
      {
         var insideObjects = 0;
         for(var s=0; s<this.objects.length; s++)
         {
            if(this.objects[s])
            {
               var posI = metadata.Convert_RD_I(this.objects[s].posRD);
               if( posI && posI.x>0 && posI.y>0 && posI.x<metadata.width && posI.y<metadata.height)
                  insideObjects++;
            }
         }
         console.writeln("<b>Catalog ", this.name, " size</b>: ", insideObjects, " objects inside the image");
      }
      else
         console.writeln("<b>Catalog ", this.name, " size</b>: ", this.objects.length, " objects");
   };

   this.CreateMagFilter = function ( field, min ,max )
   {
      if ( min != NULLMAG && max != NULLMAG )
         return "&"+field+format( "=%g..%g", min, max);
      else if ( max != NULLMAG)
         return "&"+field+format( "=<%g", max );
      if ( min != NULLMAG )
         return "&"+field+format( "=>%g", min );
      else
         return "";
   };

   this.Validate = function()
   {
      if ( this.catalogMagnitude!=null && this.magMin != NULLMAG && this.magMax != NULLMAG )
         if( this.magMin>this.magMax )
         {
            (new MessageBox( "Invalid magnitude filter: The minimum can not be greater than the maximum", TITLE, StdIcon_Error, StdButton_Ok )).execute();
            return false;
         }
      return true;
   }

   this.GetEditControls = function( parent )
   {
      if( this.filters!=null )
      {
         var magnitude_Label = new Label( parent );
         magnitude_Label.text = "Magnitude filter:";
         magnitude_Label.textAlignment = TextAlign_Right|TextAlign_VertCenter;
         magnitude_Label.minWidth = parent.labelWidth1;

         var filter_combo = null;
         if( this.filters.length>1 )
         {
            filter_combo = new ComboBox( parent );
            filter_combo.editEnabled = false;
            filter_combo.toolTip = "<p>Filter used in the magnitude test.</p>";
            filter_combo.onItemSelected = function( )
            {
               this.dialog.activeFrame.object.catalog.magnitudeFilter = filter_combo.itemText( filter_combo.currentItem );
               this.dialog.activeFrame.object.catalog.bounds = null;
            };
            for(var f=0; f<this.filters.length; f++)
            {
               filter_combo.addItem( this.filters[f] );
               if( this.filters[f] == this.magnitudeFilter )
                  filter_combo.currentItem = filter_combo.numberOfItems-1;
            }
         }

         var magnitudeMin_Edit = new Edit( parent );
         magnitudeMin_Edit.setFixedWidth( parent.editWidth );
         if ( this.magMin != NULLMAG )
            magnitudeMin_Edit.text = format( "%g", this.magMin );
         magnitudeMin_Edit.toolTip = "<p>Draw only objects with a magnitude dimmer than this value.<br/>" +
            "It can be empty.</p>";
         magnitudeMin_Edit.onTextUpdated = function( value )
         {
            if ( value != null && value.trim().length>0 )
               this.dialog.activeFrame.object.catalog.magMin = parseFloat(value);
            else
               this.dialog.activeFrame.object.catalog.magMin = NULLMAG;
            this.dialog.activeFrame.object.catalog.bounds = null;
         };

         var magnitudeMax_Edit = new Edit( parent );
         magnitudeMax_Edit.setFixedWidth( parent.editWidth );
         if ( this.magMax!=NULLMAG )
            magnitudeMax_Edit.text = format( "%g", this.magMax);
         magnitudeMax_Edit.toolTip = "<p>Draw only objects with a magnitude brighter than this value.<br />" +
            "It can be empty.</p>";
         magnitudeMax_Edit.onTextUpdated = function( value )
         {
            if ( value != null && value.trim().length>0 )
               this.dialog.activeFrame.object.catalog.magMax = parseFloat(value);
            else
               this.dialog.activeFrame.object.catalog.magMax = NULLMAG;
            this.dialog.activeFrame.object.catalog.bounds = null;
         };

         var magnitudeSeparator_Label = new Label( parent );
         magnitudeSeparator_Label.text = " - ";

         var magnitudeSizer = new HorizontalSizer;
         magnitudeSizer.spacing = 4;
         magnitudeSizer.add( magnitude_Label );
         if( filter_combo )
            magnitudeSizer.add( filter_combo );
         magnitudeSizer.add( magnitudeMin_Edit );
         magnitudeSizer.add( magnitudeSeparator_Label );
         magnitudeSizer.add( magnitudeMax_Edit );
         magnitudeSizer.addStretch();
         magnitudeSizer.setAlignment( magnitudeSeparator_Label, Align_Center );

         return [ magnitudeSizer ];
      }
      else
         return [];
   };

}


VizierCatalog.prototype = new Catalog;

VizierCatalog.mirrors = [
   {name:"CDS (vizier.u-strasbg.fr) Strasbourg, France", address:"http://vizier.u-strasbg.fr/"},
   {name:"ADAC (vizier.nao.ac.jp) Tokyo, Japan", address:"http://vizier.nao.ac.jp/"},
   {name:"CADC (vizier.hia.nrc.ca) Victoria, Canada ", address:"http://vizier.hia.nrc.ca/"},
   {name:"Cambridge (vizier.ast.cam.ac.uk) UK", address:"http://vizier.ast.cam.ac.uk/"},
   {name:"IUCAA (vizier.iucaa.ernet.in) Pune, India", address:"http://vizier.iucaa.ernet.in/"},
   {name:"INASAN (vizier.inasan.ru) Moscow, Russia", address:"http://vizier.inasan.ru/"},
   {name:"CFA Harvard (vizier.cfa.harvard.edu) Cambridge, USA", address:"http://vizier.cfa.harvard.edu/"},
   {name:"JAC (www.ukirt.jach.hawaii.edu) Hilo, Hawaii, USA", address:"http://www.ukirt.jach.hawaii.edu/"}
   ];


// ******************************************************************
// NamedStarsCatalog
// ******************************************************************
function NamedStarsCatalog()
{
   this.name="NamedStars";
   this.description = "HD-DM-GC-HR-HIP-Bayer-Flamsteed Cross Index(3690 stars)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 8;
   this.fields = [ "Name", "Coordinates", "Magnitude", "HD", "HIP", "BFD" ];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString ] );

   this.filters = [ "Vmag" ];
   this.magnitudeFilter = "Vmag";

   this.GetConstructor = function()
   {
      return "new NamedStarsCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=IV/27A/catalog IV/27A/table3&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|&-joincol=IV/27A/catalog.HD&-joincol=HD&-outjoin=IV/27A/catalog.HD"+
         "&-out=RAJ2000&-out=DEJ2000&-out=Fl&-out=Bayer&-out=Cst&-out=Name&-out=Vmag&-out=HIP";
      if ( this.magMax != NULLMAG )
         url = url + format("&Vmag=<%g",this.magMax);
      if ( this.magMin != NULLMAG )
         url = url + format("&Vmag=>%g",this.magMin);

      return url;
   }

   this.ParseRecord=function(tokens)
   {
      if ( tokens.length>=0 && parseFloat(tokens[0])>0 )
      {
         var x=DMSangle.FromString(tokens[2]).GetValue()*15;
         var y=DMSangle.FromString(tokens[3]).GetValue();
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         var BFD = tokens[5].trim();
         if( tokens[6].trim().length>0 )
            BFD = BFD ? BFD+" "+tokens[6].trim() : tokens[6].trim();
         BFD = BFD ? BFD+" "+tokens[7].trim() : tokens[7].trim();

         var name;
         if(tokens[8] && tokens[8].length>0)
            name = tokens[8];
         else if(tokens[3] && tokens[3].length>0)
            name = BFD;
         var obj = new CatalogRecord( new Point(x,y), 0, name, parseFloat(tokens[4]) );
         obj.HD = "HD"+tokens[0].trim();
         obj.hasName = tokens[8] && tokens[8].length>0;
         obj.HIP = "HIP"+tokens[1].trim();
         obj.BFD = BFD;
         return obj;
      } else
         return null;
   }

   this.PostProcessObjects = function( objects, metadata )
   {
      // Remove the objects with the same id and too near
      var tolerance = metadata.resolution*2; // 2 pixels
      var prev = null;
      for ( var i=0; i<objects.length; i++ )
      {
         if ( prev==null )
            prev = i;
         else if ( objects[i].HD==objects[prev].HD ) {
            if ( !objects[prev].hasName && objects[i].hasName ){
               objects[prev] = null;
               prev = i;
            } else
               objects[i] = null;
         } else {
            var dx = (objects[prev].posRD.x-objects[i].posRD.x)*Math.cos( objects[i].posRD.y*Math.PI/80 );
            var dy = (objects[prev].posRD.y-objects[i].posRD.y);
            var dist2 = dx*dx + dy*dy;
            if ( dist2 < tolerance*tolerance )
            {
               if ( !objects[prev].hasName && objects[i].hasName ){
                  objects[prev] = null;
                  prev = i;
               } else
                  objects[i] = null;
            } else
               prev = i;
         }
      }
   }
}

NamedStarsCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new NamedStarsCatalog );

// ******************************************************************
// HR_Catalog
// ******************************************************************
function HR_Catalog()
{
   this.name="Bright Stars ";
   this.description = "Bright Star Catalog, 5th ed. (Hoffleit+, 9110 stars)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 7;
   this.magMin = NULLMAG;
   this.magMax = 7;
   this.fields = [ "Name", "Coordinates", "HR", "HD", "DM", "SAO", "Vmag", "B-V", "U-B", "R-I", "SpType" ];

   this.properties.push(["magMin",DataType_Double]);
   this.properties.push(["magMax",DataType_Double]);
   this.properties.push(["magnitudeFilter", DataType_UCString ]);

   this.filters = [ "Vmag"];
   this.magnitudeFilter = "Vmag";

   this.GetConstructor = function()
   {
      return "new HR_Catalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=V/50/catalog&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out.add=_RAJ,_DEJ&-out=pmRA&-out=pmDE&-out=Name&-out=HR&-out=HD&-out=DM&-out=SAO"+
         "&-out=Vmag&-out=B-V&-out=U-B&-out=R-I&-out=SpType"+
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax );
      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if ( tokens.length>=14 && parseFloat( tokens[0] )>0 ) {
         var x=parseFloat( tokens[0] );
         var y=parseFloat( tokens[1] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[2] );
            var pmY = parseFloat( tokens[3] );
            var dx = pmX*(epoch-2000)/3600 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600;
            x += dx;
            y += dy;
         }
         var name = tokens[4].trim();
         if(name==null || name.length==0)
            name = "HR"+tokens[5].trim();
         var record = new CatalogRecord( new Point( x, y ), 0, name, parseFloat(tokens[9]) );
         record["HR"] = "HR"+tokens[5].trim();
         record["HD"] = "HD"+tokens[6].trim();
         record["DM"] = tokens[7].trim();
         record["SAO"] = "SAO"+tokens[8].trim();
         record["Vmag"] = tokens[9].trim();
         record["B-V"] = tokens[10].trim();
         record["U-B"] = tokens[11].trim();
         record["R-I"] = tokens[12].trim();
         record["SpType"] = tokens[13].trim();
         return record;
      } else
         return null;
   }
}

HR_Catalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new HR_Catalog );


// ******************************************************************
// HipparcosCatalog
// ******************************************************************
function HipparcosCatalog()
{
   this.name="Hipparcos";
   this.description = "Hipparcos Main catalog (118,218 stars)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 14;

   this.fields = [ "Name", "Coordinates", "Magnitude", "BT magnitude", "VT magnitude", "B-V color", "V-I index", "Spectral type", "Parallax" ];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString ] );

   this.filters = [ "VTmag", "BTmag" ];
   this.magnitudeFilter = "VTmag";

   this.GetConstructor = function()
   {
      return "new HipparcosCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/239/hip_main&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.eq=J2000&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out.add=_RAJ,_DEJ&-out=HIP&-out=Vmag&-out=Plx&-out=pmRA&-out=pmDE&-out=BTmag&-out=VTmag&-out=B-V&-out=V-I&-out=SpType" +
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax ) ;

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=11 && parseFloat(tokens[0])>0 )
      {
         var x = parseFloat( tokens[0] );
         var y = parseFloat( tokens[1] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[5] );
            var pmY = parseFloat( tokens[6] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var name="HIP"+tokens[2].trim();
         var record = new CatalogRecord( new Point( x, y ), 0, name, parseFloat( tokens[3] ) );
         record["BT magnitude"]=tokens[7].trim();
         record["VT magnitude"]=tokens[8].trim();
         record["B-V color"]=tokens[9].trim();
         record["V-I index"]=tokens[10].trim();
         record["Spectral type"]=tokens[11].trim();
         record["Parallax"]=tokens[4].trim();
         return record;
      } else
         return null;
   }
}

HipparcosCatalog.prototype = new VizierCatalog;
__catalogRegister__.Register( new HipparcosCatalog );

// ******************************************************************
// TychoCatalog
// ******************************************************************
function TychoCatalog()
{
   this.name="TYCHO-2";
   this.description = "Tycho-2 catalog (2,539,913 stars)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 16;

   this.fields = [ "Name", "Coordinates", "Magnitude", "BTmag", "HIP", "Vmag", "Bmag", "B-V index" ];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString ] );

   this.filters = [ "VTmag", "BTmag" ];
   this.magnitudeFilter = "VTmag";

   this.GetConstructor = function()
   {
      return "new TychoCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/259/tyc2&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=TYC1&-out=TYC2&-out=TYC3&-out=RAmdeg&-out=DEmdeg&-out=pmRA&-out=pmDE&-out=VTmag&-out=BTmag&-out=HIP" +
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax ) ;

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=5 && parseFloat(tokens[0])>0 )
      {
         var x = parseFloat( tokens[3] );
         var y = parseFloat( tokens[4] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[5] );
            var pmY = parseFloat( tokens[6] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var name="TYC"+tokens[0].trim()+"-"+tokens[1].trim()+"-"+tokens[2].trim();
         var record = new CatalogRecord( new Point( x, y ), 0, name, parseFloat( tokens[7] ) );
         record.VTmag = tokens[7];
         record.BTmag = tokens[8];
         if( tokens[9] )
            record.HIP = "HIP"+tokens[9].trim();
         if( tokens[7].trim().length>0 && tokens[8].trim().length>0 )
         {
            var VT = parseFloat( tokens[7] );
            var BT = parseFloat( tokens[8] );
            var V = VT-0.090*(BT-VT);
            var BV = 0.850*(BT-VT);
            var B = BV+V;
            record.Vmag = format( "%.3f", V );
            record.Bmag = format( "%.3f", B );
            record["B-V index"] = format( "%.3f", BV );
         }
         return record;
      } else
         return null;
   }
}

TychoCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new TychoCatalog );

// ******************************************************************
// PGCCatalog
// ******************************************************************
function PGCCatalog()
{
   this.name="PGC";
   this.description = "PGC HYPERLEDA I catalog of galaxies (983,261 galaxies)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.fields = [ "Name", "Coordinates" ];

   this.GetConstructor = function()
   {
      return "new PGCCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=VII/237/pgc&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=PGC&-out=RAJ2000&-out=DEJ2000&-out=logD25";
      return url;
   }

   this.ParseRecord=function(tokens)
   {
      if(tokens.length>=4 && parseFloat(tokens[0])>0){
         var x=DMSangle.FromString(tokens[1]).GetValue()*360/24;
         var y=DMSangle.FromString(tokens[2]).GetValue();
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;
         var diameter=parseFloat(tokens[3])/60*2;
         //var diameter=Math.exp(parseFloat(tokens[3]))/60;
         return new CatalogRecord(new Point(x,y),diameter,"PGC"+tokens[0].trim());
      } else
         return null;
   }
}

PGCCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new PGCCatalog );

// ******************************************************************
// PPMXCatalog
// ******************************************************************
function PPMXCatalog()
{
   this.name="PPMX";
   this.description = "PPMX catalog";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 15;
   this.magMin = NULLMAG;
   this.magMax = 15;
   this.fields = [ "Name", "Coordinates", "Cmag", "Rmag", "Bmag", "Vmag", "Jmag", "Hmag", "Kmag" ];

   this.properties.push(["magMin",DataType_Double]);
   this.properties.push(["magMax",DataType_Double]);
   this.properties.push(["magnitudeFilter", DataType_UCString ]);

   this.filters = [ "Cmag", "Rmag", "Bmag", "Vmag", "Jmag", "Hmag", "Kmag" ];
   this.magnitudeFilter = "Rmag";

   this.GetConstructor = function()
   {
      return "new PPMXCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/312/sample&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=PPMX&-out=RAJ2000&-out=DEJ2000&-out=pmRA&-out=pmDE&-out=Cmag&-out=Rmag&-out=Bmag&-out=Vmag&-out=Jmag&-out=Hmag&-out=Kmag"+
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax );
      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if ( tokens.length>=12 && parseFloat( tokens[1] )>0 ) {
         var x=parseFloat( tokens[1] );
         var y=parseFloat( tokens[2] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[3] );
            var pmY = parseFloat( tokens[4] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var record = new CatalogRecord( new Point( x, y ), 0, tokens[0].trim(), parseFloat(tokens[6]) );
         record["Cmag"] = tokens[5].trim();
         record["Rmag"] = tokens[6].trim();
         record["Bmag"] = tokens[7].trim();
         record["Vmag"] = tokens[8].trim();
         record["Jmag"] = tokens[9].trim();
         record["Hmag"] = tokens[10].trim();
         record["Kmag"] = tokens[11].trim();
         return record;
      } else
         return null;
   }
}

PPMXCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new PPMXCatalog );


// ******************************************************************
// PPMXLCatalog
// ******************************************************************
function PPMXLCatalog()
{
   this.name="PPMXL";
   this.description = "PPMXL catalog (910,469,430 objects)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 20;
   this.magMin = NULLMAG;
   this.magMax = 15;
   this.fields = [ "Name", "Coordinates", "Jmag", "Hmag", "Kmag", "b1mag", "b2mag", "r1mag", "r2mag", "imag" ];

   this.properties.push(["magMin",DataType_Double]);
   this.properties.push(["magMax",DataType_Double]);
   this.properties.push(["magnitudeFilter", DataType_UCString ]);

   this.filters = [ "Jmag", "Hmag", "Kmag", "b1mag", "b2mag", "r1mag", "r2mag", "imag" ];
   this.magnitudeFilter = "r1mag";

   this.GetConstructor = function()
   {
      return "new PPMXLCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/317&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=PPMXL&-out=RAJ2000&-out=DEJ2000&-out=pmRA&-out=pmDE"+
         "&-out=Jmag&-out=Hmag&-out=Kmag&-out=b1mag&-out=b2mag&-out=r1mag&-out=r2mag&-out=imag"+
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax );
      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if ( tokens.length>=13 && parseFloat( tokens[0] )>0 ) {
         var x=parseFloat( tokens[1] );
         var y=parseFloat( tokens[2] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[3] );
            var pmY = parseFloat( tokens[4] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var record = new CatalogRecord( new Point( x, y ), 0, tokens[0].trim(), parseFloat(tokens[10]));
         record.Jmag = tokens[5].trim();
         record.Hmag = tokens[6].trim();
         record.Kmag = tokens[7].trim();
         record.b1mag = tokens[8].trim();
         record.b2mag = tokens[9].trim();
         record.r1mag = tokens[10].trim();
         record.r2mag = tokens[11].trim();
         record.imag = tokens[12].trim();
         return record;
      } else
         return null;
   }
}

PPMXLCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new PPMXLCatalog );


// ******************************************************************
// USNOB1Catalog
// ******************************************************************
function USNOB1Catalog()
{
   this.name="USNO-B1";
   this.description = "USNO-B1.0 catalog (1,045,175,762 objects)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 20;
   this.magMax = 15;
   this.fields = [ "Name", "Coordinates", "B1mag", "B2mag", "R1mag", "R2mag", "Imag" ];

   this.properties.push(["magMin",DataType_Double]);
   this.properties.push(["magMax",DataType_Double]);
   this.properties.push(["magnitudeFilter", DataType_UCString ]);

   this.filters = [ "B1mag", "B2mag", "R1mag", "R2mag", "Imag" ];
   this.magnitudeFilter = "R1mag";

   this.GetConstructor = function()
   {
      return "new USNOB1Catalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/284/out&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=USNO-B1.0&-out=RAJ2000&-out=DEJ2000&-out=pmRA&-out=pmDE" +
         "&-out=B1mag&-out=B2mag&-out=R1mag&-out=R2mag&-out=Imag"
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax );

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if(tokens.length>=8 && parseFloat(tokens[0])>0){
         var x=parseFloat(tokens[1]);
         var y=parseFloat(tokens[2]);
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[3] );
            var pmY = parseFloat( tokens[4] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var record = new CatalogRecord( new Point( x, y ), 0, "USNO "+tokens[0].trim(), parseFloat( tokens[7] ) );
         record.B1mag = tokens[5].trim();
         record.B2mag = tokens[6].trim();
         record.R1mag = tokens[7].trim();
         if(tokens.length>8) record.R2mag = tokens[8].trim();
         if(tokens.length>9) record.Imag = tokens[9].trim();
         return record;
      } else
         return null;
   }
}

USNOB1Catalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new USNOB1Catalog );

// ******************************************************************
// UCAC3Catalog
// ******************************************************************
function UCAC3Catalog()
{
   this.name="UCAC3";
   this.description = "UCAC3 catalog (100,765,502 objects)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 15;
   this.magMax = 15;
   this.fields = [ "Name", "Coordinates", "Magnitude", "f.mag", "a.mag", "Jmag", "Hmag", "Kmag", "Bmag", "R2mag", "Imag" ];

   this.properties.push(["magMin",DataType_Double]);
   this.properties.push(["magMax",DataType_Double]);
   this.properties.push(["magnitudeFilter", DataType_UCString ]);

   this.filters = [ "f.mag", "a.mag", "Jmag", "Hmag", "Kmag", "Bmag", "R2mag", "Imag" ];
   this.magnitudeFilter = "f.mag";

   this.GetConstructor = function()
   {
      return "new UCAC3Catalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/315/out&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=3UC&-out=RAJ2000&-out=DEJ2000&-out=pmRA&-out=pmDE" +
         "&-out=f.mag&-out=a.mag&-out=Jmag&-out=Hmag&-out=Kmag&-out=Bmag&-out=R2mag&-out=Imag"+
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax );

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if(tokens.length>=6 && parseFloat(tokens[0])>0){
         var x=parseFloat(tokens[1]);
         var y=parseFloat(tokens[2]);
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[3] );
            var pmY = parseFloat( tokens[4] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var record = new CatalogRecord( new Point( x, y ), 0, "3UCAC"+tokens[0].trim(), parseFloat( tokens[5] ) );
         record["f.mag"] = tokens[5].trim();
         if(tokens.length>6) record["a.mag"] = tokens[6].trim();
         if(tokens.length>7) record.Jmag = tokens[7].trim();
         if(tokens.length>8) record.Hmag = tokens[8].trim();
         if(tokens.length>9) record.Kmag = tokens[9].trim();
         if(tokens.length>10) record.Bmag = tokens[10].trim();
         if(tokens.length>11) record.R2mag = tokens[11].trim();
         if(tokens.length>12) record.Imag = tokens[12].trim();
         return record;
      } else
         return null;
   }
}

UCAC3Catalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new UCAC3Catalog );

// ******************************************************************
// VdBCatalog
// ******************************************************************
function VdBCatalog()
{
   this.name="VdB";
   this.description = "Catalog of Reflection Nebulae - Van den Bergh (159 nebulaes)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 10.5;

   this.fields = [ "Name", "Coordinates", "Magnitude", "DM code", "Type", "Surface bright.", "Spectral type" ];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString] );

   this.filters = [ "Vmag" ];
   this.magnitudeFilter = "Vmag";

   this.GetConstructor = function()
   {
      return "new VdBCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=VII/21/catalog&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.eq=J2000&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=_RA&-out=_DE&-out=VdB&-out=DM&-out=Vmag&-out=SpType&-out=Type&-out=SurfBr&-out=BRadMax&-out=RRadMax" +
         this.CreateMagFilter( "Vmag", this.magMin, this.magMax ) ;

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=10 && parseFloat(tokens[0])>0 )
      {
         var x = parseFloat( tokens[0] );
         var y = parseFloat( tokens[1] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         var name = "VdB"+tokens[2].trim();
         var radBlue =  parseFloat( tokens[8] );
         var radRed =  parseFloat( tokens[9] );
         var radius = 0; // In arcmin
         if( radBlue && radRed)
            radius = Math.max( radBlue, radRed );
         else if( radRed )
            radius = radRed;
         else if( radBlue )
            radius = radBlue;
         var record = new CatalogRecord( new Point( x, y ), radius*2/60, name, parseFloat( tokens[4] ) );
         record["DM code"]=tokens[3].trim();
         record["Type"]=tokens[6].trim();
         record["Surface brightness"]=tokens[7].trim();
         record["Spectral type"]=tokens[5].trim();
         return record;
      } else
         return null;
   }
}

VdBCatalog.prototype = new VizierCatalog;
__catalogRegister__.Register( new VdBCatalog );

// ******************************************************************
// SharplessCatalog
// ******************************************************************
function SharplessCatalog()
{
   this.name="Sharpless";
   this.description = "Catalog of HII Regions - Sharpless (313 nebulaes)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.fields = [ "Name", "Coordinates" ];

   this.GetConstructor = function()
   {
      return "new SharplessCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=VII/20/catalog&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.eq=J2000&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out.add=_RAJ,_DEJ&-out=Sh2&-out=Diam";

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=4 && parseFloat(tokens[0])>0 )
      {
         var x = parseFloat( tokens[0] );
         var y = parseFloat( tokens[1] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         var name = "Sh2-"+tokens[2].trim();
         var diam =  parseFloat( tokens[3] );
         if(!diam)
            diam = 0;
         var record = new CatalogRecord( new Point( x, y ), diam/60, name );
         return record;
      } else
         return null;
   }
}

SharplessCatalog.prototype = new VizierCatalog;
__catalogRegister__.Register( new SharplessCatalog );

// ******************************************************************
// BarnardCatalog
// ******************************************************************
function BarnardCatalog()
{
   this.name="Barnard";
   this.description = "Barnard's Catalog of Dark Objects in the Sky (349 objects)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.fields = [ "Name", "Coordinates" ];

   this.GetConstructor = function()
   {
      return "new BarnardCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=VII/220A/barnard&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.eq=J2000&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out.add=_RAJ,_DEJ&-out=Barn&-out=Diam";

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=4 && parseFloat(tokens[0])>0 )
      {
         var x = parseFloat( tokens[0] );
         var y = parseFloat( tokens[1] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         var name = "B"+tokens[2].trim();
         var diam =  parseFloat( tokens[3] );
         if(!diam)
            diam = 0;
         var record = new CatalogRecord( new Point( x, y ), diam/60, name );
         return record;
      } else
         return null;
   }
}

BarnardCatalog.prototype = new VizierCatalog;
__catalogRegister__.Register( new BarnardCatalog );

// ******************************************************************
// B-V White Balance Stars from NOMAD1
// ******************************************************************
// hacked by Troy Piggins from the Hipparcos function above

function BVCatalog()
{
   this.name="NOMAD-1 B-V WB";
   this.description = "NOMAD-1 star catalog with B-V filtering for white balance";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 14;
   this.bvMin = 0.6;
   this.bvMax = 0.7;
   this.vrMin = 0.2;
   this.vrMax = 0.6;

   this.fields = [ "Name", "Coordinates", "Vmag", "Bmag", "Rmag", "B-V index", "V-R index" ];
   this.filters = [ "Vmag", "Bmag", "Rmag" ];
   this.magnitudeFilter = "Vmag";

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["bvMin", DataType_Double] );
   this.properties.push( ["bvMax", DataType_Double] );
   this.properties.push( ["vrMin", DataType_Double] );
   this.properties.push( ["vrMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString] );

   this.GetConstructor = function()
   {
      return "new BVCatalog()";
   }

   this._base_GetEditControls = this.GetEditControls;
   this.GetEditControls = function( parent )
   {
      var controls = this._base_GetEditControls( parent );

      // B-V filter
      var bv_Label = new Label( parent );
      bv_Label.text = "B-V filter:";
      bv_Label.textAlignment = TextAlign_Right|TextAlign_VertCenter;
      bv_Label.minWidth = parent.labelWidth1;

      var bvMin_Edit = new Edit( parent );
      bvMin_Edit.setFixedWidth( parent.editWidth );
      if ( this.bvMin != NULLMAG )
         bvMin_Edit.text = format( "%g", this.bvMin );
      bvMin_Edit.toolTip = "<p>Draw only objects with a B-V index greater than this value.<br/>" +
         "It can be empty.</p>";
      bvMin_Edit.onTextUpdated = function( value )
      {
         if ( value != null && value.trim().length>0 )
            this.dialog.activeFrame.object.catalog.bvMin = parseFloat(value);
         else
            this.dialog.activeFrame.object.catalog.bvMin = NULLMAG;
         this.dialog.activeFrame.object.catalog.bounds=null;
      };

      var bvMax_Edit = new Edit( parent );
      bvMax_Edit.setFixedWidth( parent.editWidth );
      if ( this.bvMax!=NULLMAG )
         bvMax_Edit.text = format( "%g", this.bvMax);
      bvMax_Edit.toolTip = "<p>Draw only objects with a B-V index lower than this value.<br />" +
         "It can be empty.</p>";
      bvMax_Edit.onTextUpdated = function( value )
      {
         if ( value != null && value.trim().length>0 )
            this.dialog.activeFrame.object.catalog.bvMax = parseFloat(value);
         else
            this.dialog.activeFrame.object.catalog.bvMax = NULLMAG;
         this.dialog.activeFrame.object.catalog.bounds=null;
      };

      var bvSeparator_Label = new Label( parent );
      bvSeparator_Label.text = " - ";

      // V-R filter
      var vr_Label = new Label( parent );
      vr_Label.text = "V-R filter:";
      vr_Label.textAlignment = TextAlign_Right|TextAlign_VertCenter;

      var vrMin_Edit = new Edit( parent );
      vrMin_Edit.setFixedWidth( parent.editWidth );
      if ( this.vrMin != NULLMAG )
         vrMin_Edit.text = format( "%g", this.vrMin );
      vrMin_Edit.toolTip = "<p>Draw only objects with a V-R index greater than this value.<br/>" +
         "It can be empty.</p>";
      vrMin_Edit.onTextUpdated = function( value )
      {
         if ( value != null && value.trim().length>0 )
            this.dialog.activeFrame.object.catalog.vrMin = parseFloat(value);
         else
            this.dialog.activeFrame.object.catalog.vrMin = NULLMAG;
         this.dialog.activeFrame.object.catalog.bounds=null;
      };

      var vrMax_Edit = new Edit( parent );
      vrMax_Edit.setFixedWidth( parent.editWidth );
      if ( this.vrMax!=NULLMAG )
         vrMax_Edit.text = format( "%g", this.vrMax);
      vrMax_Edit.toolTip = "<p>Draw only objects with a V-R index lower than this value.<br />" +
         "It can be empty.</p>";
      vrMax_Edit.onTextUpdated = function( value )
      {
         if ( value != null && value.trim().length>0 )
            this.dialog.activeFrame.object.catalog.vrMax = parseFloat(value);
         else
            this.dialog.activeFrame.object.catalog.vrMax = NULLMAG;
         this.dialog.activeFrame.object.catalog.bounds=null;
      };

      var vrSeparator_Label = new Label( parent );
      vrSeparator_Label.text = " - ";


      bvSizer = new HorizontalSizer;
      bvSizer.spacing = 4;
      bvSizer.add( bv_Label );
      bvSizer.add( bvMin_Edit );
      bvSizer.add( bvSeparator_Label );
      bvSizer.add( bvMax_Edit );
      bvSizer.addSpacing( 4 );
      bvSizer.add( vr_Label );
      bvSizer.add( vrMin_Edit );
      bvSizer.add( vrSeparator_Label );
      bvSizer.add( vrMax_Edit );
      bvSizer.addStretch();
      bvSizer.setAlignment( bvSeparator_Label, Align_Center );
      bvSizer.setAlignment( vrSeparator_Label, Align_Center );

      controls.push( bvSizer );
      return controls;
   };

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/297&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.eq=J2000&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out.add=_RAJ,_DEJ&-out=NOMAD1&-out=Vmag&-out=Bmag&-out=Rmag&-out=pmRA&-out=pmDE&-out=R" +
         this.CreateMagFilter( this.magnitudeFilter, (this.magMin==NULLMAG) ? -5 : this.magMin, (this.magMax==NULLMAG) ? 25 : this.magMax);
      if( this.magnitudeFilter != "Vmag")
         url += this.CreateMagFilter( "Vmag", -5, 25 );
      if( this.magnitudeFilter != "Bmag")
         url += this.CreateMagFilter( "Bmag", -5, 25 );
      if( this.magnitudeFilter != "Rmag")
         url += this.CreateMagFilter( "Rmag", -5, 25 );

      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=8 && parseFloat(tokens[0])>0 )
      {
         var recommended = tokens[8].trim();
         // Exclude problematic stars
         if( tokens[8].trim()=="*" )
            return null;

         // Get magnitude values
         var V = parseFloat( tokens[3] ); // Returns NaN if it doesn't exist
         var B = parseFloat( tokens[4] ); // Returns NaN if it doesn't exist
         var R = parseFloat( tokens[5] ); // Returns NaN if it doesn't exist

         // Calculate B-V
         var BV = B-V;

         // Calculate V-R
         var VR = V-R;

         // Filter by B-V index
         if( (this.bvMin!=NULLMAG && BV<this.bvMin) || (this.bvMax!=NULLMAG && BV>this.bvMax) )
            return null;

         // Filter by V-R index
         if( (this.vrMin!=NULLMAG && VR<this.vrMin) || (this.vrMax!=NULLMAG && VR>this.vrMax) )
            return null;

         var x = parseFloat( tokens[0] );
         var y = parseFloat( tokens[1] );
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null )
         {
            var pmX = parseFloat( tokens[6] );
            var pmY = parseFloat( tokens[7] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var name = tokens[2].trim();
         var record = new CatalogRecord( new Point( x, y ), 0, name, parseFloat( tokens[3] ) );
         record.Vmag = tokens[3].trim();
         record.Bmag = tokens[4].trim();
         record.Rmag = tokens[5].trim();
         record["B-V index"] = format( "%.3f", BV );
         record["V-R index"] = format( "%.3f", VR );

         return record;
      } else
         return null;
   }
}

BVCatalog.prototype = new VizierCatalog;
__catalogRegister__.Register( new BVCatalog );

// ******************************************************************
// SDSSCatalog
// ******************************************************************
function SDSSCatalog()
{
   this.name="SDSS";
   this.description = "SDSS R8 catalog (469,053,874 objects)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 25;

   this.fields = [ "Name", "Coordinates", "Magnitude", "Class", "Redshift", "umag", "gmag", "rmag", "imag", "zmag"];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString ] );
   this.properties.push( ["classFilter", DataType_UInt16 ] );

   this.filters = [ "umag", "gmag", "rmag", "imag", "zmag" ];
   this.magnitudeFilter = "rmag";
   this.classFilter = 0;

   this.GetConstructor = function()
   {
      return "new SDSSCatalog()";
   }

   this._base_GetEditControls = this.GetEditControls;
   this.GetEditControls = function( parent )
   {
      var controls = this._base_GetEditControls( parent );

      // Class filter
      var class_Label = new Label( parent );
      class_Label.text = "Class:";
      class_Label.textAlignment = TextAlign_Right|TextAlign_VertCenter;
      class_Label.minWidth = parent.labelWidth1;

      var class_combo = new ComboBox( parent );
      class_combo.editEnabled = false;
      class_combo.toolTip = "<p>Filter the objects of the catalog by class.</p>";
      class_combo.onItemSelected = function( )
      {
         this.dialog.activeFrame.object.catalog.classFilter = class_combo.currentItem;
         this.dialog.activeFrame.object.catalog.bounds=null;
      };
      class_combo.addItem( "All objects" );
      class_combo.addItem( "Stars" );
      class_combo.addItem( "Galaxies" );
      class_combo.currentItem = this.classFilter;

      classSizer = new HorizontalSizer;
      classSizer.spacing = 4;
      classSizer.add( class_Label );
      classSizer.add( class_combo );
      classSizer.addStretch( );

      controls.push( classSizer );
      return controls;
   };

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=II/306/sdss8&mode==1&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=SDSS8&-out=RAJ2000&-out=DEJ2000&-out=pmRA&-out=pmDE&-out=cl&-out=zsp" +
         "&-out=umag&-out=gmag&-out=rmag&-out=imag&-out=zmag" +
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax ) ;
      if( this.classFilter==1 )
         url += "&cl==6";
      else if( this.classFilter==2 )
         url += "&cl==3";
      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=12 && parseFloat(tokens[1])>0 )
      {
         var x=parseFloat(tokens[1]);
         var y=parseFloat(tokens[2]);
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         if( epoch!=null && tokens[3].trim().length>0 && tokens[4].trim().length>0 )
         {
            var pmX = parseFloat( tokens[3] );
            var pmY = parseFloat( tokens[4] );
            var dx = pmX*(epoch-2000)/3600000 * Math.cos( y*Math.PI/180 );
            var dy = pmY*(epoch-2000)/3600000;
            x += dx;
            y += dy;
         }
         var record = new CatalogRecord( new Point( x, y ), 0, "SDSS8"+tokens[0].trim(), 0 );
         record.Redshift = tokens[6].trim();
         record.Class = tokens[5].trim();
         record.umag = tokens[7].trim();
         record.gmag = tokens[8].trim();
         record.rmag = tokens[9].trim();
         record.imag = tokens[10].trim();
         record.zmag = tokens[11].trim();
         record.magnitude = parseFloat( record[this.magnitudeFilter] );
         return record;
      } else
         return null;
   }
}

SDSSCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new SDSSCatalog );

// ******************************************************************
// GSCCatalog
// ******************************************************************
function GSCCatalog()
{
   this.name="GSC";
   this.description = "GSC2.3 catalog (945,592,683 objects)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 23;

   this.fields = [ "Name", "Coordinates", "Magnitude", "Class", "Fmag", "jmag", "Vmag", "Nmag", "Umag", "Bmag"];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString ] );
   this.properties.push( ["classFilter", DataType_UInt16 ] );

   this.filters = [ "Fmag", "jmag", "Vmag", "Nmag", "Umag", "Bmag" ];
   this.magnitudeFilter = "jmag";
   this.classFilter = 0;

   this.GetConstructor = function()
   {
      return "new GSCCatalog()";
   }

   this._base_GetEditControls = this.GetEditControls;
   this.GetEditControls = function( parent )
   {
      var controls = this._base_GetEditControls( parent );

      // Class filter
      var class_Label = new Label( parent );
      class_Label.text = "Class:";
      class_Label.textAlignment = TextAlign_Right|TextAlign_VertCenter;
      class_Label.minWidth = parent.labelWidth1;

      var class_combo = new ComboBox( parent );
      class_combo.editEnabled = false;
      class_combo.toolTip = "<p>Filter the objects of the catalog by class.</p>";
      class_combo.onItemSelected = function( )
      {
         this.dialog.activeFrame.object.catalog.classFilter = class_combo.currentItem;
         this.dialog.activeFrame.object.catalog.bounds=null;
      };
      class_combo.addItem( "All objects" );
      class_combo.addItem( "Star" );
      class_combo.addItem( "Non-star" );
      class_combo.currentItem = this.classFilter;

      classSizer = new HorizontalSizer;
      classSizer.spacing = 4;
      classSizer.add( class_Label );
      classSizer.add( class_combo );
      classSizer.addStretch( );

      controls.push( classSizer );
      return controls;
   };

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/305/out&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=GSC2.3&-out=RAJ2000&-out=DEJ2000&-out=Class" +
         "&-out=Fmag&-out=jmag&-out=Vmag&-out=Nmag&-out=Umag&-out=Bmag" +
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax ) ;
      if( this.classFilter==1 )
         url += "&Class==0";
      else if( this.classFilter==2 )
         url += "&Class==3";
      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=9 && parseFloat(tokens[1])>0 )
      {
         var x=parseFloat(tokens[1]);
         var y=parseFloat(tokens[2]);
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         var record = new CatalogRecord( new Point( x, y ), 0, tokens[0].trim(), 0 );
         record.Class = tokens[3].trim();
         record.Fmag = tokens[4].trim();
         record.jmag = tokens[5].trim();
         record.Vmag = tokens[6].trim();
         record.Nmag = tokens[7].trim();
         record.Umag = tokens[8].trim();
         if(tokens.length>9) record.Bmag = tokens[9].trim();
         record.magnitude = parseFloat( record[this.magnitudeFilter] );
         return record;
      } else
         return null;
   }
}

GSCCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new GSCCatalog );


// ******************************************************************
// CMC14Catalog
// ******************************************************************
function CMC14Catalog()
{
   this.name="CMC14";
   this.description = "CMC14 catalog (95,858,475 stars)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 17;

   this.fields = [ "Name", "Coordinates", "Magnitude", "Class", "r'mag", "Jmag", "Hmag", "Ksmag" ];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString ] );

   this.filters = [ "r'mag", "Jmag", "Hmag", "Ksmag" ];
   this.magnitudeFilter = "r'mag";
   this.classFilter = 0;

   this.GetConstructor = function()
   {
      return "new CMC14Catalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=I/304/out&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=CMC14&-out=RAJ2000&-out=DEJ2000" +
         "&-out=r'mag&-out=Jmag&-out=Hmag&-out=Ksmag" +
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax ) ;
      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=2 && parseFloat(tokens[1])>0 )
      {
         var x=parseFloat(tokens[1]);
         var y=parseFloat(tokens[2]);
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         var record = new CatalogRecord( new Point( x, y ), 0, tokens[0].trim(), 0 );
         if(tokens.length>3) record["r'mag"] = tokens[3].trim();
         if(tokens.length>4) record.Jmag = tokens[4].trim();
         if(tokens.length>5) record.Hmag = tokens[5].trim();
         if(tokens.length>6) record.Ksmag = tokens[6].trim();
         record.magnitude = parseFloat( record[this.magnitudeFilter] );
         return record;
      } else
         return null;
   }
}

CMC14Catalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new CMC14Catalog );

// ******************************************************************
// ARPCatalog
// ******************************************************************
function ARPCatalog()
{
   this.name="ARP";
   this.description = "ARP catalog (592 galaxies)";

   this.__base__ = VizierCatalog;
   this.__base__( this.name );

   this.catalogMagnitude = 17;

   this.fields = [ "Name", "CommonName", "Coordinates", "Magnitude", "MType", "VTmag" ];

   this.properties.push( ["magMin", DataType_Double] );
   this.properties.push( ["magMax", DataType_Double] );
   this.properties.push( ["magnitudeFilter", DataType_UCString ] );

   this.filters = [ "VTmag" ];
   this.magnitudeFilter = "VTmag";

   this.GetConstructor = function()
   {
      return "new ARPCatalog()";
   }

   this.UrlBuilder = function(metadata, mirrorServer)
   {
      var fieldOfView=Math.max(metadata.width,metadata.height)*metadata.resolution;
      var url=mirrorServer+"viz-bin/asu-tsv?-source=VII/192/arplist&-c=" +
         format("%f %f",metadata.ra,metadata.dec) +
         "&-c.r=" + format("%f",fieldOfView) +
         "&-c.u=deg&-out.form=|"+
         "&-out=Arp&-out=RA2000&-out=DE2000" +
         "&-out=Name&-out=VT&-out=dim1&-out=MType" +
         this.CreateMagFilter( this.magnitudeFilter, this.magMin, this.magMax ) ;
      return url;
   }

   this.ParseRecord = function( tokens, epoch )
   {
      if( tokens.length>=2 && parseFloat(tokens[1])>0 )
      {
         var x=DMSangle.FromString(tokens[1]).GetValue()*360/24;
         var y=DMSangle.FromString(tokens[2]).GetValue();
         if( !(x>=0 && x<=360 && y>=-90 && y<=90) )
            return null;

         var diameter = parseFloat(tokens[5])/60;
         var record = new CatalogRecord( new Point( x, y ), diameter, "ARP"+tokens[0].trim());
         record["CommonName"] = tokens[3].trim();
         record["VTmag"] = tokens[4].trim();
         record["MType"] = tokens[6].trim();
         record.magnitude = parseFloat( record[this.magnitudeFilter] );
         return record;
      } else
         return null;
   }
}

ARPCatalog.prototype=new VizierCatalog;
__catalogRegister__.Register( new ARPCatalog );

// ******************************************************************
// CustomCatalog: Uses a file to store the info
// ******************************************************************
function CustomCatalog()
{
   this.name = "Custom Catalog";
   this.description = "User defined catalog";

   this.__base__ = Catalog;
   this.__base__( this.name );

   this.catalogPath = null;

   this.fields = [ "Name", "Coordinates", "Magnitude" ];
   this.properties.push(["catalogPath", DataType_String]);

   this.GetConstructor = function()
   {
      return "new CustomCatalog()";
   }

   this.Validate = function()
   {
      if(!this.catalogPath || this.catalogPath.trim().length==0){
         (new MessageBox( "The path of the custom catalog is empty", TITLE, StdIcon_Error, StdButton_Ok )).execute();
         return false;
      }
      if( !File.exists( this.catalogPath ) ){
         (new MessageBox( "The file of the custom catalog doesn't exist", TITLE, StdIcon_Error, StdButton_Ok )).execute();
         return false;
      }

      var catalogLines = this.LoadLines();

      if( catalogLines.length==0 )
      {
         new MessageBox( "The custom catalog is empty", TITLE, StdIcon_Error ).execute();
         return false;
      }

      return this.ParseHeader(catalogLines[0]) != null;
   }

   this.LoadLines = function()
   {
      var file = new File();
      file.openForReading( this.catalogPath );
      if( !file.isOpen )
      {
         new MessageBox( "The custom catalog file could not be opened", TITLE, StdIcon_Error, StdButton_Ok ).execute();
         return [];
      }

      var fileData = file.read( DataType_ByteArray,file.size );
      file.close();
      var str = fileData.toString();
      if( str.indexOf( "\r\n" ) >=0 )
         return str.split( "\r\n" );
      else if( str.indexOf( "\r" ) >=0 )
         return str.split( "\r" );
      else
         return str.split( "\n" );
   }

   this.ParseHeader = function( headerLine )
   {
      if( !headerLine )
      {
         new MessageBox( "The header line is empty", TITLE, StdIcon_Error ).execute();
         return null;
      }
      var index = {};

      var fields = headerLine.split("\t");
      fields = fields.map( function(s) { return s.trim().toLowerCase(); } );
      //console.writeln(fields);
      index.ra  = fields.indexOf( "ra" );
      index.dec = fields.indexOf( "dec" );
      index.dia = fields.indexOf( "diameter" );
      index.mag = fields.indexOf( "magnitude" );
      index.nam = fields.indexOf( "name" );

      if( index.ra<0 || index.dec<0 )
      {
         new MessageBox( "The custom catalog hasn't valid coordinate's columns", TITLE, StdIcon_Error ).execute();
         return null;
      }

      return index;
   }

   this.Load = function( metadata )
   {
      var file = new File();
      if( !this.catalogPath )
         return false;

      var catalogLines = this.LoadLines();
      if( catalogLines.length==0 )
      {
         new MessageBox( "The custom catalog is empty", TITLE, StdIcon_Error ).execute();
         return false;
      }

      var index = this.ParseHeader( catalogLines[0] );
      if( index==null )
         return false;

      var minLength = Math.max( index.ra, index.dec ) + 1;

      this.objects = new Array;
      var numWarnings=0;
      for( var i=1; i<catalogLines.length; i++ )
      {
         if( catalogLines[i].trim().length==0 )
            continue;
         var fields = catalogLines[i].split("\t");

         if( fields.length < minLength )
         {
            if( numWarnings<50 )
               console.writeln("Warning: The line ", i+1, " doesn't contain coordinates");
            numWarnings++;
            continue;
         }

         var x = parseFloat( fields[index.ra] );
         var y = parseFloat( fields[index.dec] );

         var diameter = 0;
         if( index.dia>=0 && fields.length>index.dia )
            diameter = parseFloat( fields[index.dia] )/60;

         var name;
         if( index.nam>=0 && fields.length>index.nam )
            name = fields[index.nam].trim();

         var magnitude;
         if( index.mag>=0 && fields.length>index.mag  )
            magnitude = parseFloat( fields[index.mag] );

         this.objects.push( new CatalogRecord( new Point(x,y), diameter, name, magnitude ) );
      }
      if( numWarnings>50 )
         console.writeln( "Total number of warnings: ", numWarnings );
      console.writeln( "\n<b>Custom catalog size</b>: ", this.objects.length, " objects" );

      return true;
   };

   this.GetEditControls = function( parent )
   {
      // Catalog path
      var path_Label = new Label( parent );
      path_Label.text = "Catalog path:";
      path_Label.textAlignment = TextAlign_Right|TextAlign_VertCenter;

      var path_Edit = new Edit(parent);
      path_Edit.text = this.catalogPath ? this.catalogPath : "";
      path_Edit.onTextUpdated = function( value ) { this.dialog.activeFrame.object.catalog.catalogPath = value; };

      var path_Button = new ToolButton( parent );
      path_Button.icon = ":/icons/select-file.png";
      path_Button.toolTip = "<p>Select the NGC/IC catalog file.</p>";
      path_Button.onClick = function()
      {
         var gdd = new OpenFileDialog;
         if ( this.dialog.activeFrame.object.catalog.catalogPath )
            gdd.initialPath = this.dialog.activeFrame.object.catalog.catalogPath;
         gdd.caption = "Select Custom Catalog Path";
         gdd.filters = [["Text files", "*.txt"]];
         if ( gdd.execute() )
         {
            this.dialog.activeFrame.object.catalog.catalogPath = gdd.fileName;
            path_Edit.text = gdd.fileName;
         }
      };

      var pathSizer = new HorizontalSizer;
      pathSizer.spacing = 4;
      pathSizer.add(path_Label);
      pathSizer.add(path_Edit,100);
      pathSizer.add(path_Button);

      return [pathSizer];
   };
}

CustomCatalog.prototype = new Catalog;
__catalogRegister__.Register( new CustomCatalog );
