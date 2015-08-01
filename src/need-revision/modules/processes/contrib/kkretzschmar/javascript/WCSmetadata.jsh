/*
   WCS metadata class

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

#define Ext_DataType_Complex     1000  // Complex object with settings
#define Ext_DataType_StringArray 1001  // Array of strings
#define Ext_DataType_JSON        1002  // Serializable object

// ******************************************************************
// ObjectWithSettings: Base class for persistent classes
// ******************************************************************
function ObjectWithSettings( module, prefix, properties )
{
   this.module = module;
   this.prefix = prefix;
   this.properties = properties;

   this.MakeSettingsKey = function( property )
   {
      var key = "";
      if( this.module && this.module.length>0 )
         key = this.module + "/";
      if( this.prefix && prefix.length>0 )
         key = key + this.prefix + "/";
      return key + property;
   }

   this.LoadSettings = function()
   {
      for ( var i=0; i<this.properties.length; i++ )
      {
         var property = this.properties[i][0];
         //console.writeln("Reading ", this.MakeSettingsKey( property ) );
         if(property)
         {
            if( this.properties[i][1] == Ext_DataType_Complex ){
               if(this[property] && typeof(this[property].LoadSettings)==='function')
                  this[property].LoadSettings();
            }
            else if( this.properties[i][1] == Ext_DataType_JSON )
            {
               var value = Settings.read( this.MakeSettingsKey( property ), DataType_UCString );
               // console.writeln(Settings.lastReadOK, " ", value );
               if ( Settings.lastReadOK )
                  this[property] = JSON.parse(value);
            }
            else if( this.properties[i][1] == Ext_DataType_StringArray )
            {
               var value = Settings.read( this.MakeSettingsKey( property ), DataType_UCString );
               //console.writeln("Reading ", this.MakeSettingsKey( property ), " ", value );
               if ( Settings.lastReadOK )
                  this[property] = value.split("|");
            }
            else
            {
               var value = Settings.read( this.MakeSettingsKey( property ), this.properties[i][1] );
               // console.writeln(Settings.lastReadOK, " ", value );
               if ( Settings.lastReadOK )
                  this[property] = value;
            }
         }
      }
   };

   this.SaveSettings = function()
   {
      for ( var i=0; i<this.properties.length; i++ )
      {
         var property = this.properties[i][0];
         //console.writeln("Writing ", this.MakeSettingsKey( property ) );
         if ( this[property] != null )
         {
            if( this.properties[i][1] == Ext_DataType_Complex )
               this[property].SaveSettings();
            else if( this.properties[i][1] == Ext_DataType_JSON )
               Settings.write( this.MakeSettingsKey( property ), DataType_UCString, JSON.stringify(this[property]) );
            else if (this.properties[i][1] == Ext_DataType_StringArray)
            {
               var concatString = this.CreateStringArray(this[property]);
               if (concatString != null)
                  Settings.write(this.MakeSettingsKey(property), DataType_UCString, concatString);
            }
            else
               Settings.write( this.MakeSettingsKey( property ), this.properties[i][1], this[property] );
         }
         else
            Settings.remove( this.MakeSettingsKey( property ) );
      }
   };

   this.DeleteSettings = function()
   {
      Settings.remove( this.prefix );
   };

   this.MakeParamsKey = function( property )
   {
      var key = "";
      if( this.prefix && this.prefix.length>0 )
         key = this.prefix.replace("-","") + "_";

      return key + property;
   }

   this.LoadParameters = function()
   {
      for ( var i=0; i<this.properties.length; i++ )
      {
         var property = this.properties[i][0];
         // console.writeln("Reading ", this.MakeParamsKey( property ) );
         if(property)
         {
            if( this.properties[i][1] == Ext_DataType_Complex )
               this[property].LoadParameters();
            else
            {
               var key = this.MakeParamsKey( property );
               if( Parameters.has(key) ){
                  // console.writeln(key," = ", Parameters.get( key ));
                  switch(this.properties[i][1])
                  {
                     case DataType_Boolean:
                        this[property] = Parameters.getBoolean( key );
                        break;
                     case DataType_Int8:
                     case DataType_UInt8:
                     case DataType_Int16:
                     case DataType_UInt16:
                     case DataType_Int32:
                     case DataType_UInt32:
                     case DataType_Int64:
                     case DataType_UInt64:
                        this[property] = parseInt( Parameters.get( key ) );
                        break;
                     case DataType_Double:
                     case DataType_Float:
                        this[property] = Parameters.getReal( key );
                        break;
                     case DataType_String:
                     case DataType_UCString:
                        this[property] = Parameters.getString( key );
                        break;
                  case Ext_DataType_JSON:
                        var valStrEscaped = Parameters.getString(key);
                        // TODO: This is necessary because PI 1.8 doesn't allow " in strings
                        var valStr = valStrEscaped.replace( /\'\'/g, "\"" );
                        this[property] = JSON.parse(valStr);
                        break;
                     case Ext_DataType_StringArray:
                        var value = Parameters.getString( key );
                        if ( value )
                           this[property] = value.split("|");
                        break;
                     default:
                        console.writeln("Unknown type ",this.properties[i][1]);
                  }
               }
            }
         }
      }
   }

   this.SaveParameters = function()
   {
      for ( var i=0; i<this.properties.length; i++ )
      {
         var property = this.properties[i][0];
         // console.writeln("Writing ", this.MakeParamsKey( property ) );
         if ( this[property] != null )
         {
            if( this.properties[i][1] == Ext_DataType_Complex )
               this[property].SaveParameters();
            else if( this.properties[i][1] == Ext_DataType_JSON )
            {
               var valStr = JSON.stringify(this[property]);
               // TODO: This is necessary because PI 1.8 doesn't allow " in strings
               var valStrEscaped = valStr.replace( /\"/g, "\'\'" );
               Parameters.set(this.MakeParamsKey(property), valStrEscaped);
            }
            else if( this.properties[i][1] == Ext_DataType_StringArray )
            {
               var array = this.CreateStringArray(this[property]);
               if (array != null)
                  Parameters.set(this.MakeParamsKey(property), array);
            }
            else
               Parameters.set( this.MakeParamsKey( property ), this[property] );
         }
      }
   };

   this.CreateStringArray = function( array )
   {
      var str = null;
      for(var j=0; j<array.length; j++)
         if( array[j] )
            str = ( str==null) ? array[j] : str+"|"+array[j];
         else
            str = ( str==null) ? "" : str+"|";
      return str;
   }
}

// ******************************************************************
// ImageMetadata: Metadata of an image including WCS coordinates
// ******************************************************************
function ImageMetadata(module)
{
   this.__base__ = ObjectWithSettings;
   this.__base__(
      module ? module : SETTINGS_MODULE,
      "metadata",
      new Array(
         ["focal", DataType_Double],
         ["hasFocal", DataType_Boolean],
         ["xpixsz", DataType_Float],
         // ["ypixsz", DataType_Float],
         ["resolution", DataType_Double],
         ["ra", DataType_Double],
         ["dec", DataType_Double],
         ["epoch", DataType_Double]
      )
   );

   this.focal = 1000;
   this.useFocal = true;
   this.xpixsz = 7.4;
   // this.ypixsz = 7.4;
   this.epoch = null; // 2451545.0; // J2000
   this.ra = 0;
   this.dec = 0;

   this.Clone = function()
   {
      var clone = new SolverConfiguration();
      for ( var field in this )
         clone[field] = this[field];
      return clone;
   };

   this.ExtractMetadata = function(window)
   {
      var objctra, objctdec;
      var focallen, xpixsz;
      var crval1, crval2;
      var crpix1, crpix2;
      var cd1_1, cd1_2;
      var cd2_1, cd2_2;
      var cdelt1, cdelt2;
      var crota1, crota2;
      var ctype1, ctype2;
      var polDegree;

      var keywords = window.keywords;
      for( var i=0; i<keywords.length; i++ )
      {
         var key = keywords[i];
#ifdef DEBUG
         Console.writeln(key.name,": ",key.value);
#endif
         if ( key.name == "OBJCTRA" )
            objctra = DMSangle.FromString(key.value, 0, 24).GetValue()*360/24;
         if ( key.name == "OBJCTDEC" )
            objctdec = DMSangle.FromString(key.value, 0, 90).GetValue();
         if ( key.name == "FOCALLEN" )
            focallen = parseFloat(key.value);
         if ( key.name == "XPIXSZ" )
            this.xpixsz = parseFloat(key.value);
         if ( key.name == "DATE-OBS" )
         {
            var date = this.ExtractDate(key.value);
            if( date )
               this.epoch = date;
         }

         // if ( key.name == "EPOCH" )
            // epoch=parseFloat(key.value);
         if ( key.name == "CTYPE1" )
            ctype1 = key.value.trim();
         if ( key.name == "CTYPE2" )
            ctype2 = key.value.trim();
         if ( key.name == "CRVAL1" )
            crval1 = parseFloat(key.value);
         if ( key.name == "CRVAL2" )
            crval2 = parseFloat(key.value);
         if ( key.name == "CRPIX1" )
            crpix1 = parseFloat(key.value);
         if ( key.name == "CRPIX2" )
            crpix2 = parseFloat(key.value);
         if ( key.name == "CD1_1" )
            cd1_1 = parseFloat(key.value);
         if ( key.name == "CD1_2" )
            cd1_2 = parseFloat(key.value);
         if ( key.name == "CD2_1" )
            cd2_1 = parseFloat(key.value);
         if ( key.name == "CD2_2" )
            cd2_2 = parseFloat(key.value);

         if ( key.name == "CDELT1" )
            cdelt1 = parseFloat(key.value);
         if ( key.name == "CDELT2" )
            cdelt2 = parseFloat(key.value);
         if ( key.name == "CROTA1" )
            crota1 = parseFloat(key.value);
         if ( key.name == "CROTA2" )
            crota2 = parseFloat(key.value);

         if ( key.name == "POLYNDEG" )
            polDegree = parseInt(key.value);
      }

      this.width = window.mainView.image.width;
      this.height = window.mainView.image.height;

      if ( ctype1 && ctype1.substr(0,9) == "'RA---TAN" &&
           ctype2 &&ctype2.substr(0,9) == "'DEC--TAN" &&
           crpix1 != null && crpix2 != null && crval1 != null && crval2 != null )
      {
         this.projection = new Gnomonic( 180/Math.PI, crval1, crval2 );
         var ref_F_G;
         if ( cd1_1!=null && cd1_2!=null && cd2_1!=null && cd2_2!=null )
         {
#ifdef DEBUG
            console.writeln("WCS referentiation");
#endif
            ref_F_G = new Matrix(
                  cd1_1, cd1_2, -cd1_1*crpix1 - cd1_2*crpix2,
                  cd2_1, cd2_2, -cd2_1*crpix1 - cd2_2*crpix2,
                  0, 0, 1);
         }
         else if ( cdelt1 != null && cdelt2 != null && crota2 != null)
         {
#ifdef DEBUG
            console.writeln("AIPS referentiation");
#endif
            var rot = crota2*Math.PI/180;
            var cd1_1 = cdelt1*Math.cos(rot);
            var cd1_2 = -cdelt2*Math.sin(rot);
            var cd2_1 = cdelt1*Math.sin(rot);
            var cd2_2 = cdelt2*Math.cos(rot);
            ref_F_G = new Matrix(
                  cd1_1, cd1_2, -cd1_1*crpix1 - cd1_2*crpix2,
                  cd2_1, cd2_2, -cd2_1*crpix1 - cd2_2*crpix2,
                  0, 0, 1);
         }
         if( ref_F_G != null )
         {
            var ref_F_I = new Matrix(
               1,0,-0.5,
               0,-1,this.height+0.5,
               0,0,1
            );

            if(polDegree!=null && polDegree>1)
            {
               this.ref_I_G_lineal = ref_F_G.mul(ref_F_I.inverse());
               this.ref_I_G = new ReferNPolyn(polDegree);
               this.ref_G_I = new ReferNPolyn(polDegree);

               try{
                  var idx=0;
                  for(var o=0; o<=this.ref_I_G.polDegree; o++)
                  {
                     for (var yi = 0; yi <= o; yi++)
                     {
                        var xi = o-yi;
                        this.ref_I_G.at(0, idx, this.GetKeywordFloat(keywords, format("REFX_%d%d", xi, yi), true));
                        this.ref_I_G.at(1, idx, this.GetKeywordFloat(keywords, format("REFY_%d%d", xi, yi), true));
                        this.ref_G_I.at(0, idx, this.GetKeywordFloat(keywords, format("INVX_%d%d", xi, yi), true));
                        this.ref_G_I.at(1, idx, this.GetKeywordFloat(keywords, format("INVY_%d%d", xi, yi), true));
                        idx++;
                     }
                  }
               } catch(ex){
                  console.writeln("Invalid advanced referentiation: ", ex);
                  this.ref_I_G = this.ref_I_G_lineal;
                  this.ref_G_I = this.ref_I_G.inverse();
               }
            } else{
               this.ref_I_G_lineal = ref_F_G.mul(ref_F_I.inverse());
               this.ref_I_G = this.ref_I_G_lineal;
               this.ref_G_I = this.ref_I_G.inverse();
            }

            var centerG = this.ref_I_G.Apply (new Point( this.width/2, this.height/2 ) );
            var center=this.projection.Inverse( centerG );
            this.ra=center.x;
            this.dec=center.y;

            var resx = Math.sqrt(ref_F_G.at(0, 0) * ref_F_G.at(0, 0) + ref_F_G.at(0, 1) * ref_F_G.at(0, 1));
            var resy = Math.sqrt(ref_F_G.at(1, 0) * ref_F_G.at(1, 0) + ref_F_G.at(1, 1) * ref_F_G.at(1, 1));
            this.resolution = (resx + resy) / 2;
            this.useFocal = false;
            if ( this.xpixsz > 0 )
               this.focal = this.FocalFromResolution( this.resolution );
         }
      }

      if( this.ref_I_G == null )
      {
#ifdef DEBUG
            console.writeln("No referentiation");
#endif
         if( objctra != null )
            this.ra = objctra;
         if( objctdec != null )
            this.dec = objctdec;
         if( focallen>0 )
         {
            this.focal = focallen;
            this.useFocal = true;
         }
         if( this.useFocal && this.xpixsz > 0 )
            this.resolution = this.ResolutionFromFocal( this.focal );
      }
   }

   this.ExtractDate = function( timeStr )
   {
      var match = timeStr.match("'?([0-9]*)-([0-9]*)-([0-9]*)(T([0-9]*):([0-9]*):([0-9]*(\.[0-9]*)?))?'?");
      if( match==null)
         return null;
      var year = parseInt( match[1], 10 );
      var month = parseInt( match[2], 10 );
      var day = parseInt( match[3], 10 );
      var hour = match[5] ? parseInt( match[5], 10 ) : 0;
      var min = match[6] ? parseInt( match[6], 10 ) : 0;
      var sec = match[7] ? parseFloat( match[7] ) : 0;
      var frac = (hour+min/60+sec/3600) / 24;

      return Math.complexTimeToJD( year, month, day, frac );
   };

   this.GetDateString = function( epoch )
   {
      var dateArray = Math.jdToComplexTime(epoch);
      var hours=Math.floor(dateArray[3]*24);
      var min=Math.floor(dateArray[3]*24*60)-hours*60;
      var sec=dateArray[3]*24*3600-hours*3600-min*60;
      var dateStr=format("%04d-%02d-%02dT%02d:%02d:%0.2f",dateArray[0],dateArray[1],dateArray[2],hours,min,sec);
      return dateStr;
   }

   this.ResolutionFromFocal = function( focal )
   {
      return (focal > 0) ? this.xpixsz/focal*0.18/Math.PI : 0;
   };

   this.FocalFromResolution = function( resolution )
   {
      return (resolution > 0) ? this.xpixsz/resolution*0.18/Math.PI : 0;
   };

   this.GetWCSvalues = function()
   {
      var ref_F_I = new Matrix(
               1,  0, -0.5,
               0, -1, this.height+0.5,
               0,  0, 1
            );
      var ref_F_G;
      if (this.ref_I_G.polDegree && this.ref_I_G.polDegree != 1)
         ref_F_G = this.ref_I_G_lineal.mul(ref_F_I);
      else
      {
         if (this.ref_I_G.ToLinealMatrix)
            ref_F_G = this.ref_I_G.ToLinealMatrix().mul(ref_F_I);
         else
            ref_F_G = this.ref_I_G.mul(ref_F_I);
      }

      //ref_F_G.Print();

      this.cd1_1 = ref_F_G.at( 0, 0 );
      this.cd1_2 = ref_F_G.at( 0, 1 );
      this.cd2_1 = ref_F_G.at( 1, 0 );
      this.cd2_2 = ref_F_G.at( 1, 1 );

      var orgF = ref_F_G.inverse().Apply(new Point(0,0));
      this.crpix1 = orgF.x;
      this.crpix2 = orgF.y;
      this.crval1 = this.projection.ra0*180/Math.PI;
      this.crval2 = this.projection.dec0*180/Math.PI;

	  // CDELT1, CDELT2 and CROTA2 are computed using the formulas
	  // in section 6.2 of http://fits.gsfc.nasa.gov/fits_wcs.html
	  // "Representations of celestial coordinates in FITS"

      var rot1, rot2;

      if ( this.cd2_1 > 0 )
         rot1 = Math.atan2( this.cd2_1, this.cd1_1 );
      else if ( this.cd2_1 < 0 )
         rot1 = Math.atan2( -this.cd2_1, -this.cd1_1 );
      else
         rot1 = 0;

      if ( this.cd1_2 > 0 )
         rot2 = Math.atan2( this.cd1_2, -this.cd2_2 );
      else if ( this.cd1_2 < 0 )
         rot2 = Math.atan2(-this.cd1_2,this.cd2_2);
      else
         rot2 = 0;

      var rot = (rot1 + rot2)/2;
      rot2 = rot1 = rot;

      if ( Math.abs( Math.cos( rot ) ) > Math.abs( Math.sin( rot ) ) )
      {
         this.cdelt1 =  this.cd1_1/Math.cos( rot );
         this.cdelt2 =  this.cd2_2/Math.cos( rot );
      }
      else
      {
         this.cdelt1 =  this.cd2_1/Math.sin( rot );
         this.cdelt2 = -this.cd1_2/Math.sin( rot );
      }

      this.crota1 = rot1*180/Math.PI;
      this.crota2 = rot2*180/Math.PI;
   };

   this.GetRotation = function()
   {
      var ref = this.ref_I_G_lineal ? this.ref_I_G_lineal : this.ref_I_G;
      var det = ref.at( 0, 1 )*ref.at( 1, 0 ) - ref.at( 0, 0 )*ref.at( 1, 1 );
      var rot = Math.atan2( ref.at( 0, 0 ) + ref.at( 0, 1 ), ref.at( 1, 0 ) + ref.at( 1, 1 ) )*180/Math.PI + 135;
      if ( det > 0 )
         rot = -90 - rot;
      if ( rot < -180 )
         rot += 360;
      if ( rot > 180 )
         rot -= 360;
      return [rot, det>0];
   }

   this.Print = function()
   {
      var ref = this.ref_I_G_lineal ? this.ref_I_G_lineal : this.ref_I_G;
      console.writeln( "Referentiation Matrix (Gnomonic projection = Matrix * Coords[x,y]):" );
      ref.Print();
      var projOrgPx=this.ref_G_I.Apply(new Point(0,0));
      var projOrgRD = new Point(this.projection.ra0*180/Math.PI, this.projection.dec0*180/Math.PI);
      console.writeln( format(    "Projection origin.. [%.6f %.6f]pix -> [RA:%ls Dec:%ls]",
         projOrgPx.x,projOrgPx.y,
         DMSangle.FromAngle(projOrgRD.x*24/360).ToString(),DMSangle.FromAngle(projOrgRD.y).ToString()) );
      if(this.ref_I_G.polDegree && this.ref_I_G.polDegree>1)
         console.writeln(  format("Polynomial degree.. %d", this.ref_I_G.polDegree) );
      console.writeln( format(    "Resolution ........ %.3f arcsec/pix", this.resolution*3600 ) );
      var rotation = this.GetRotation();
      console.writeln( format(    "Rotation .......... %.3f deg", rotation[0] ), rotation[1] ? " (flipped)" : "" );

      if ( this.xpixsz > 0 && this.focal)
      {
         console.writeln( format( "Focal ............. %.2f mm", this.focal ) );
         console.writeln( format( "Pixel size ........ %.2f um", this.xpixsz ) );
      }

      console.writeln( "Field of view ..... ",     this.FieldString( this.width*this.resolution ), " x ", this.FieldString( this.height*this.resolution ) );

      console.write(   "Image center ...... ");  this.PrintCoords( new Point( this.width/2, this.height/2) );
      console.writeln( "Image bounds:" );
      console.write(   "   top-left ....... " ); this.PrintCoords( new Point( 0,          0           ) );
      console.write(   "   top-right ...... " ); this.PrintCoords( new Point( this.width, 0           ) );
      console.write(   "   bottom-left .... " ); this.PrintCoords( new Point( 0,          this.height ) );
      console.write(   "   bottom-right ... " ); this.PrintCoords( new Point( this.width, this.height ) );
   }

   this.PrintCoords = function ( pI )
   {
      var pRD = this.Convert_I_RD( pI );
      var ra_val = pRD.x;
      if ( ra_val < 0 )
         ra_val += 360;
      var ra = DMSangle.FromAngle( ra_val*24/360 );
      var dec = DMSangle.FromAngle( pRD.y );
      console.writeln( "RA: ", ra.ToString( true/*hours*/ ), "  Dec: ", dec.ToString() );
   }

   this.FieldString = function( field )
   {
      var dms = DMSangle.FromAngle( field );
      if ( dms.deg > 0 )
         return format( "%dd %d' %.1f\"", dms.deg, dms.min, dms.sec );
      if ( dms.min > 0 )
         return format( "%d' %.1f\"", dms.min, dms.sec );
      return format( "%.2f\"", dms.sec );
   }

   this.ModifyKeyword = function( keywords, name, value, comment )
   {
      for ( var i=0; i<keywords.length; i++ )
      {
         if ( keywords[i].name == name )
         {
            keywords[i].value = value;
            if ( comment != null )
               keywords[i].comment = comment;
            return;
         }
      }
      keywords.push( new FITSKeyword( name, value, (comment == null) ? "" : comment ) );
   }

   this.GetKeywordFloat = function (keywords, name, exception)
   {
      for (var i = 0; i < keywords.length; i++)
         if (keywords[i].name == name)
            return parseFloat(keywords[i].value);
      if (exception)
         throw format("Keyword %ls not found", name);
      return null;
   }

   this.UpdateBasicKeywords = function(keywords)
   {
      if(this.focal>0)
         this.ModifyKeyword( keywords, "FOCALLEN", format( "%.2f", this.focal ),                                        "Focal Length (mm)");
      if(this.xpixsz>0)
      {
         this.ModifyKeyword( keywords, "XPIXSZ",   format( "%.3f", this.xpixsz ),                                       "Pixel size, X-axis (um)" );
         this.ModifyKeyword( keywords, "YPIXSZ",   format( "%.3f", this.xpixsz ),                                       "Pixel size, Y-axis (um)" );
      }
      this.ModifyKeyword( keywords, "OBJCTRA",  '\'' + DMSangle.FromAngle( this.ra*24/360 ).ToString( true ) + '\'', "Image center R.A. (hms)" );
      this.ModifyKeyword( keywords, "OBJCTDEC", '\'' + DMSangle.FromAngle( this.dec ).ToString()             + '\'', "Image center declination (dms)" );
   }

   this.UpdateWCSKeywords = function(keywords)
   {
      this.GetWCSvalues();

      this.ModifyKeyword( keywords, "EQUINOX", "2000",                            "Equatorial equinox" );
      this.ModifyKeyword( keywords, "CTYPE1",  "'RA---TAN'",                      "Axis1 projection: Gnomonic" );
      this.ModifyKeyword( keywords, "CTYPE2",  "'DEC--TAN'",                      "Axis2 projection: Gnomonic" );
      this.ModifyKeyword( keywords, "CRPIX1",  format( "%.6f", this.crpix1 ),       "Axis1 reference pixel" );
      this.ModifyKeyword( keywords, "CRPIX2",  format( "%.6f", this.crpix2 ),       "Axis2 reference pixel" );
      this.ModifyKeyword( keywords, "CRVAL1",  format( "%.12g", this.crval1 ),    "Axis1 reference value" );
      this.ModifyKeyword( keywords, "CRVAL2",  format( "%.12g", this.crval2 ),    "Axis2 reference value" );
      this.ModifyKeyword( keywords, "CD1_1",   format( "%.12g", this.cd1_1 ),     "Scale matrix (1,1)" );
      this.ModifyKeyword( keywords, "CD1_2",   format( "%.12g", this.cd1_2 ),     "Scale matrix (1,2)" );
      this.ModifyKeyword( keywords, "CD2_1",   format( "%.12g", this.cd2_1 ),     "Scale matrix (2,1)" );
      this.ModifyKeyword( keywords, "CD2_2",   format( "%.12g", this.cd2_2 ),     "Scale matrix (2,2)" );

      // deprecated
      //this.ModifyKeyword( keywords, "EPOCH",   format( "%.3f", this.epoch ),        "Epoch of coordinates" );

      // AIPS keywords (CDELT1, CDELT2, CROTA1, CROTA2)
      this.ModifyKeyword( keywords, "CDELT1",  format( "%.12g", this.cdelt1 ),    "Axis1 scale" );
      this.ModifyKeyword( keywords, "CDELT2",  format( "%.12g", this.cdelt2 ),    "Axis2 scale" );
      this.ModifyKeyword( keywords, "CROTA1",  format( "%.12g", this.crota1 ),    "Axis1 rotation angle (deg)" );
      this.ModifyKeyword( keywords, "CROTA2",  format( "%.12g", this.crota2 ),    "Axis2 rotation angle (deg)" );
   }

   this.UpdateReferKeywords = function(keywords)
   {
      if(!this.ref_I_G.polDegree || this.ref_I_G.polDegree==1){
         this.ModifyKeyword( keywords, "POLYNDEG",  "1",    "Polynomial degree" );
         return;
      }
      this.ModifyKeyword( keywords, "POLYNDEG",  this.ref_I_G.polDegree.toString(),    "Polynomial degree" );
      var idx=0;
      for(var o=0; o<=this.ref_I_G.polDegree; o++)
      {
         for (var yi = 0; yi <= o; yi++)
         {
            var xi = o-yi;
            this.ModifyKeyword( keywords, format("REFX_%d%d",xi,yi), format( "%.12g", this.ref_I_G.at(0,idx)), format("CoefX * x^%d * y^%d", xi,yi) );
            this.ModifyKeyword( keywords, format("REFY_%d%d",xi,yi), format( "%.12g", this.ref_I_G.at(1,idx)), format("CoefY * x^%d * y^%d", xi,yi) );
            this.ModifyKeyword( keywords, format("INVX_%d%d",xi,yi), format( "%.12g", this.ref_G_I.at(0,idx)), format("InvCoefX * x^%d * y^%d", xi,yi) );
            this.ModifyKeyword( keywords, format("INVY_%d%d",xi,yi), format( "%.12g", this.ref_G_I.at(1,idx)), format("InvCoefY * x^%d * y^%d", xi,yi) );
            idx++;
         }
      }
   }

   this.RectExpand = function( r, p )
   {
      r.x0 = Math.min(r.x0, p.x);
      r.x1 = Math.max(r.x1, p.x);
      r.y0 = Math.min(r.y0, p.y);
      r.y1 = Math.max(r.y1, p.y);
   }

   this.FindImageBounds = function()
   {
      var center = this.Convert_I_RD( new Point( this.width/2, this.height/2 ) );
      var bounds = new Rect( center.x, center.y, center.x, center.y );

      this.RectExpand( bounds, this.Convert_I_RD( new Point(0,0)));
      this.RectExpand( bounds, this.Convert_I_RD( new Point(this.width/2,0)));
      this.RectExpand( bounds, this.Convert_I_RD( new Point(this.width,0)));
      this.RectExpand( bounds, this.Convert_I_RD( new Point(this.width,this.height/2)));
      this.RectExpand( bounds, this.Convert_I_RD( new Point(this.width,this.height)));
      this.RectExpand( bounds, this.Convert_I_RD( new Point(this.width/2,this.height)));
      this.RectExpand( bounds, this.Convert_I_RD( new Point(0,this.height)));
      this.RectExpand( bounds, this.Convert_I_RD( new Point(0,this.height/2)));

      if( this.projection.dec0>0 )
      {
         // Check North Pole
         var north_I = this.Convert_RD_I( new Point( this.projection.ra0*180/Math.PI, 90 ) );
         if( north_I.x>=0 && north_I.x<this.width && north_I.y>=0 && north_I.y<this.height )
         {
            bounds.x0 = 0; bounds.x1 = 360;
            bounds.y1 = 90;
         }
      }
      else
      {
         // Check South Pole
         var south_I = this.Convert_RD_I( new Point( this.projection.ra0*180/Math.PI, -90 ) );
         if( south_I.x>=0 && south_I.x<this.width && south_I.y>=0 && south_I.y<this.height )
         {
            bounds.x0 = 0; bounds.x1 = 360;
            bounds.y0 = -90;
         }
      }

      bounds.x0 *= 24/360;
      bounds.x1 *= 24/360;

      return bounds;
   }

   this.Convert_I_RD = function( pI )
   {
      return this.projection.Inverse( this.ref_I_G.Apply( pI ) );
   };

   this.Convert_RD_I = function( pRD )
   {
      var pG = this.projection.Direct( pRD );
      return pG ? this.ref_G_I.Apply( pG ) : null;
   };
}

ImageMetadata.prototype = new ObjectWithSettings;

// ******************************************************************
// DMSangle: Helper class for simplifying the use of angles in DMS format
// ******************************************************************
function DMSangle()
{
   this.deg = 0;
   this.min = 0;
   this.sec = 0;
   this.sign = 1;

   this.GetValue = function()
   {
      return this.sign*(this.deg + this.min/60 + this.sec/3600);
   };

   this.ToString = function( hours )
   {
      var plus = hours ? "" : "+";
      if ( this.deg != null && this.min != null && this.sec != null && this.sign != null )
         return ((this.sign < 0) ? "-": plus) +
               format( "%02d %02d %0*.*f", this.deg, this.min, hours ? 6 : 5, hours ? 3 : 2, this.sec );
      return "<* invalid *>";
   };
}

DMSangle.FromString = function( coordStr, mindeg, maxdeg )
{
   var match = coordStr.match( "'?([+-]?)([0-9]*) ([0-9]*) ([0-9]*(.[0-9]*)?)'?" );
   if( match==null )
      return null;
   var coord = new DMSangle();
   if ( match.length < 4 )
      throw new Error( "Invalid coordinates" );
   coord.deg = parseInt( match[2], 10 );
   if ( coord.deg < mindeg || coord.deg > maxdeg )
      throw new Error( "Invalid coordinates" );
   coord.min = parseInt( match[3], 10 );
   if ( coord.min < 0 || coord.min >= 60 )
      throw new Error( "Invalid coordinates (minutes)" );
   coord.sec = parseFloat( match[4] );
   if ( coord.sec < 0 || coord.sec >= 60 )
      throw new Error( "Invalid coordinates (seconds)" );
   coord.sign = (match[1] == '-') ? -1 : 1;
   return coord;
}

DMSangle.FromAngle = function( angle )
{
   var coord = new DMSangle();
   if ( angle < 0 )
   {
      coord.sign = -1;
      angle = -angle;
   }
   coord.deg = Math.floor( angle );
   coord.min = Math.floor( (angle - coord.deg)*60 );
   coord.sec = (angle - coord.deg - coord.min/60)*3600;

   if( coord.sec > 59.999 ){
      coord.sec = 0;
      coord.min++;
      if( coord.min == 60 ){
         coord.min = 0;
         coord.deg++;
      }
   }

   return coord;
}

// ******************************************************************
// Gnomonic: Gnomonic projection
// ******************************************************************
function Gnomonic( scale, ra0, dec0 )
{
   this.scale = scale;
   this.ra0 = ra0*Math.PI/180;
   this.dec0 = dec0*Math.PI/180;

   this.sinDec0 = Math.sin( this.dec0 );
   this.cosDec0 = Math.cos( this.dec0 );

   this.Direct = function (p)
   {
      var ra = p.x * Math.PI / 180;
      var dec = p.y * Math.PI / 180;
      var sinDec = Math.sin(dec);
      var cosDec = Math.cos(dec);
      var cosRa = Math.cos(ra - this.ra0);

      // Coordinate validation
      if (this.ra0 - ra > Math.PI)
         ra += Math.PI * 2;
      else if (this.ra0 - ra < -Math.PI)
         ra -= Math.PI * 2;
      var dist = Math.acos(this.sinDec0 * sinDec + this.cosDec0 * cosDec * cosRa);
      if (dist > Math.PI / 2)
         return null;

      var A = cosDec * cosRa;
      var F = scale / ( this.sinDec0 * sinDec + A * this.cosDec0 );
      return new Point(F * cosDec * Math.sin(ra - this.ra0),
         F * (this.cosDec0 * sinDec - A * this.sinDec0));
   };

   this.Inverse = function (p)
   {
      var X = -p.x / this.scale;
      var Y = -p.y / this.scale;
      var D = Math.atan(Math.sqrt(X * X + Y * Y));
      var B = Math.atan2(-X, Y);
      var sinD = Math.sin(D);
      var cosD = Math.cos(D);
      var cosB = Math.cos(B);
      var XX = this.sinDec0 * sinD * cosB + this.cosDec0 * cosD;
      var YY = sinD * Math.sin(B);
      var ra = this.ra0 + Math.atan2(YY, XX);
      var dec = Math.asin(this.sinDec0 * cosD - this.cosDec0 * sinD * cosB);
      return new Point(ra * 180 / Math.PI, dec * 180 / Math.PI);
   };
}

Point.prototype.PrintAsRaDec = function()
{
   console.writeln("RA: ",DMSangle.FromAngle(this.x*24/360).ToString(),
      "  Dec: ",DMSangle.FromAngle(this.y).ToString());
};

Point.prototype.Print = function()
{
   console.writeln( format( "%f %f", this.x, this.y ) );
};

Matrix.prototype.Apply = function( p )
{
   var matrixP = new Matrix( [p.x, p.y, 1], 3, 1 );
   var p1 = this.mul( matrixP );
   return new Point( p1.at( 0, 0 ), p1.at( 1, 0 ) );
};

Matrix.prototype.Print = function Print()
{
   for ( var y = 0; y < this.rows; y++ )
   {
      console.write( "   " );
      for ( var x = 0; x < this.cols; x++ )
         //console.write( format( "%+20.12f", this.at( y, x ) ) );
         console.write( format( "%+20g", this.at( y, x ) ) );
      console.writeln( "" );
   }
};

function ReferNPolyn(polDegree)
{
   this.__base__ = Matrix;
   this.__base__(2, ((polDegree + 1) * (polDegree + 2)) / 2);
   this.polDegree = polDegree;
};
ReferNPolyn.prototype = new Matrix;

ReferNPolyn.prototype.Apply = function (p)
{
/*   var matrixP = new Matrix(this.GetPointCoef(p), this.GetNumCoef(), 1);
   var p1 = this.mul(matrixP);
   //console.writeln(p,"||",matrixP.toString(),"||",p1.toString());
   return new Point(p1.at(0, 0), p1.at(1, 0));*/
   var coef=this.GetPointCoef(p);
   var x=0;
   var y=0;
   for(var i=0; i<coef.length; i++)
   {
      x+=coef[i]*this.at(0,i);
      y+=coef[i]*this.at(1,i);
   }
   return new Point(x,y);
}

ReferNPolyn.prototype.GetPointCoef = function (p)
{
/*   var values = [];
   var y = 1;
   for (var yi = 0; yi <= this.polDegree; yi++)
   {
      var x = 1;
      for (var xi = 0; xi + yi <= this.polDegree; xi++)
      {
         values.push(x * y);
         x *= p.x;
      }
      y *= p.y;
   }
   //console.writeln(p," ",values);
   return values;*/
   var values = Array(this.GetNumCoef());
   var idx=0;
   for(var o=0; o<=this.polDegree; o++)
   {
/*      for (var yi = 0; yi <= o; yi++)
      {
         var xi = o-yi;
         values[idx++]=Math.pow(p.x,xi) * Math.pow(p.y,yi);
      }*/
      var x= 1;
      for(var i=0; i<=o; i++)
      {
         values[idx+o-i]=x;
         x*= p.x;
      }
      var y=1;
      for(var i=0; i<=o; i++)
      {
         values[idx+i]*=y;
         y*= p.y;
      }
      idx+=o+1;
   }
   return values;
}

ReferNPolyn.prototype.GetNumCoef = function (degree)
{
   if(degree==null)
      return ((this.polDegree + 1) * (this.polDegree + 2)) / 2;
   else
      return ((degree + 1) * (degree + 2)) / 2;
}

ReferNPolyn.prototype.ToLinealMatrix = function()
{
   var m=new Matrix(3,3);
   m.at(0,0, this.at(0,1)); m.at(0,1, this.at(0,2)); m.at(0,2, this.at(0,0));
   m.at(1,0, this.at(1,1)); m.at(1,1, this.at(1,2)); m.at(1,2, this.at(1,0));
   m.at(2,0, 0);            m.at(2,1, 0);            m.at(2,2, 1);
   return m;
}

ReferNPolyn.prototype.FromLinealMatrix = function(m)
{
   var ref=new ReferNPolyn(1);
   ref.at(0, 0, m.at(0,2)); ref.at(0, 1, m.at(0,0)); ref.at(0, 2, m.at(0,1));
   ref.at(1, 0, m.at(1,2)); ref.at(1, 1, m.at(1,0)); ref.at(1, 2, m.at(1,1));
   return ref;
}

Matrix.prototype.toString = function()
{
   var str="[";
   for(var row=0; row<this.rows; row++)
   {
      var rowStr="[";
      for(var col=0; col<this.columns; col++)
      {
         if(col>0)
            rowStr+=";";
         rowStr+=this.at(row,col).toString();
      }
      str+=rowStr+"]";
   }
   return str+="]";
}

function MultipleLinearRegression(polDegree, coords1, coords2)
{
   if (coords1.length != coords2.length)
      throw "Input arrays of different size in Multiple Linear Regression";
   var numSamples =0;
   for(var i=0; i<coords1.length; i++)
      if(coords1[i] && coords2[i])
         numSamples++;
   //console.writeln("Samples: ", numSamples);
   if(numSamples<4)
      throw "There are too few valid samples";
   // Uses independent multiple linear regression for x and y
   // The model is: Y = X * B + err
   // The regresand Y contains the x (or y) of the predicted coordinates coords2
   // The regresors X contains the vectors (x,y,1) with the source coordinates coords1
   // The parameter vector B contains the factors of the expression xc = xi*B0 + yi*B1 + B2
   var ref_1_2 = new ReferNPolyn(polDegree);
   var numCoefs=ref_1_2.GetNumCoef();
   var Y1 = new Matrix(numSamples, 1);
   var Y2 = new Matrix(numSamples, 1);
   var X = new Matrix(numSamples, numCoefs);
   var row=0;
   for (var i = 0; i < coords1.length; i++)
   {
      if(coords1[i] && coords2[i])
      {
         //console.writeln(coords1[i]," ",coords2[i]);
         Y1.at(row, 0, coords2[i].x);
         Y2.at(row, 0, coords2[i].y);

         var Xval = ref_1_2.GetPointCoef(coords1[i]);
         for(var c=0; c<numCoefs; c++)
            X.at(row, c, Xval[c]);
         row++;
      }
   }

   // Solve the two multiple regressions
   var XT = X.transpose();
   var XT_X_inv_XT=(XT.mul(X)).inverse().mul(XT);
   var B1 = XT_X_inv_XT.mul(Y1);
   var B2 = XT_X_inv_XT.mul(Y2);

   // Create the correction matrix that transform from coords1 to coords2
   //console.writeln("B1:"); B1.Print();
   //console.writeln("B2:"); B2.Print();
   for(var i=0; i<numCoefs; i++)
   {
      ref_1_2.at(0, i, B1.at(i,0));
      ref_1_2.at(1, i, B2.at(i,0));
   }
   //console.writeln("Correction matrix:");
   //ref_1_2.Print();

   // Calculate R2 and RMS
/*   var SSR=0;
   for(var i=0; i<coords1.length; i++)
   {
      if(coords1[i] && coords2[i])
      {
         var c2 = ref_1_2.Apply(coords1[i]);
         var errX=c2.x-coords2[i].x;
         var errY=c2.y-coords2[i].y;
         //console.writeln(format("%f;%f;%f;%f",coords1[i].x,coords1[i].y,errX,errY));
         SSR+=errX*errX+errY*errY;
      }
   }
   var RMSerr=Math.sqrt(SSR/numSamples);*/

   //return { ref_1_2: ref_1_2, rms: RMSerr};
   return ref_1_2;
};
