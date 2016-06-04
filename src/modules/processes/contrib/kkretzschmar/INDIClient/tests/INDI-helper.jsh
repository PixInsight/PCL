#ifndef _INDI_helper_jsh
#define _INDI_helper_jsh

#include <pjsr/DataType.jsh>

/*
 * Loads all FITS keywords from the first HDU of the specified FITS file and
 * returns them as an array.
 */
if ( !File.loadFITSKeywordArray )
   File.loadFITSKeywordArray = function( filePath )
   {
      function searchCommentSeparator( b )
      {
         var inString = false;
         for ( var i = 9; i < 80; ++i )
            switch ( b.at( i ) )
            {
            case 39: // single quote
               inString ^= true;
               break;
            case 47: // slash
               if ( !inString )
                  return i;
               break;
            }
         return -1;
      }

      var f = new File;
      f.openForReading( filePath );

      var keywords = new Array;
      for ( ;; )
      {
         var rawData = f.read( DataType_ByteArray, 80 );

         var name = rawData.toString( 0, 8 );
         if ( name.toUpperCase() == "END     " ) // end of HDU keyword list?
            break;

         if ( f.isEOF )
            throw new Error( "Unexpected end of file: " + filePath );

         var value;
         var comment;
         if ( rawData.at( 8 ) == 61 ) // value separator (an equal sign at byte 8) present?
         {
            // This is a valued keyword
            var cmtPos = searchCommentSeparator( rawData ); // find comment separator slash
            if ( cmtPos < 0 ) // no comment separator?
               cmtPos = 80;
            value = rawData.toString( 9, cmtPos-9 ); // value substring
            if ( cmtPos < 80 )
               comment = rawData.toString( cmtPos+1, 80-cmtPos-1 ); // comment substring
            else
               comment = new String;
         }
         else
         {
            // No value in this keyword
            value = new String;
            comment = rawData.toString( 8, 80-8 );
         }

         // Perform a naive sanity check: a valid FITS file must begin with a SIMPLE=T keyword.
         if ( keywords.length == 0 )
            if ( name != "SIMPLE  " && value.trim() != 'T' )
               throw new Error( "File does not seem a valid FITS file: " + filePath );

         // Add new keyword. Note: use FITSKeyword with PI >= 1.6.1
         keywords.push({ name:name.trim(), value: value.trim(), comment:comment.trim() } );
      }
      f.close();
      return keywords;
   };

function indexOfFITSKeyword( keywords, keywordName )
{
   for ( let index = 0; index < keywords.length; ++index )
      if ( keywords[index].name == keywordName )
         return index;
   return -1;
}

function indexOfDevice( deviceTable, deviceName )
{
   for ( let row = 0; row < deviceTable.length; ++row )
      if ( deviceTable[row][0] == deviceName )
         return row;
   return -1;
}

function propertyEquals( propertyTable, propertyKey, propertyValue )
{
   for ( let row = 0; row < propertyTable.length; ++row )
      if ( propertyTable[row][0] == propertyKey )
         if ( propertyTable[row][1] == propertyValue )
            return true;
   return false;
}

#endif //_INDI_helper_jsh
