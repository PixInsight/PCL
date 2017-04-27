

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/DrizzleData.h>
#include <pcl/XML.h>

#include <errno.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void DrizzleData::Clear()
{
   m_sourceFilePath = m_cfaSourceFilePath = m_cfaSourcePattern = m_alignTargetFilePath = String();
   m_referenceWidth = m_referenceHeight = -1;
   m_H = Matrix();
   m_S.Clear();
   m_Sx = m_Sy = spline();
   ClearIntegrationData();
}

void DrizzleData::ClearIntegrationData()
{
   m_location = m_referenceLocation = m_scale = m_unitScale = m_weight = m_unitWeight = Vector();
   m_rejectionLowCount = m_rejectionHighCount = UI64Vector();
   m_rejectLowData = m_rejectHighData = rejection_data();
   m_rejectionMap.FreeData();
}

// ----------------------------------------------------------------------------

XMLDocument* DrizzleData::Serialize() const
{
   // Validate image registration data
   if ( m_sourceFilePath.IsEmpty() ||
        m_referenceWidth < 1 || m_referenceHeight < 1 ||
       !m_H.IsEmpty() && (m_H.Rows() != 3 || m_H.Columns() != 3) ||
        m_H.IsEmpty() && !m_S.IsValid() )
      throw Error( "Invalid/insufficient image registration data." );

   // Validate image integration data
   if ( m_location.Length() != m_referenceLocation.Length() ||
       !m_scale.IsEmpty()  && m_location.Length() != m_scale.Length() ||
       !m_weight.IsEmpty() && m_location.Length() != m_weight.Length() ||
       !m_rejectHighData.IsEmpty() && size_type( m_location.Length() ) != m_rejectHighData.Length() ||
       !m_rejectLowData.IsEmpty()  && size_type( m_location.Length() ) != m_rejectLowData.Length() )
      throw Error( "Invalid/insufficient image integration data." );

   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight XML Drizzle Data Format - XDRZ version 1.0"
                           "\nCreated with PixInsight software - http://pixinsight.com/"
                           "\n" );

   XMLElement* root = new XMLElement( "xdrz", XMLAttributeList()
      << XMLAttribute( "version", "1.0" )
      << XMLAttribute( "xmlns", "http://www.pixinsight.com/xdrz" )
      << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
      << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xdrz http://pixinsight.com/xdrz/xdrz-1.0.xsd" ) );

   xml->SetRootElement( root );

   *(new XMLElement( *root, "CreationTime" )) << new XMLText( TimePoint::Now().ToString() );

   *(new XMLElement( *root, "SourceImage" )) << new XMLText( m_sourceFilePath );

   if ( !m_cfaSourceFilePath.IsEmpty() )
      *(new XMLElement( *root, "CFASourceImage", XMLAttributeList()
            << (m_cfaSourcePattern.IsEmpty() ? XMLAttribute() : XMLAttribute( "pattern", m_cfaSourcePattern )) )
       ) << new XMLText( m_cfaSourceFilePath );

   if ( !m_alignTargetFilePath.IsEmpty() )
      *(new XMLElement( *root, "AlignmentTargetImage" )) << new XMLText( m_alignTargetFilePath );

   new XMLElement( *root, "ReferenceGeometry", XMLAttributeList()
      << XMLAttribute( "width", String( m_referenceWidth ) )
      << XMLAttribute( "height", String( m_referenceHeight ) )
      << (m_location.IsEmpty() ? XMLAttribute() : XMLAttribute( "numberOfChannels", String( m_location.Length() ) )) );

   if ( !m_H.IsEmpty() )
      *(new XMLElement( *root, "AlignmentMatrix" )) << new XMLText( String().ToCommaSeparated( m_H ) );

   if ( m_S.IsValid() )
   {
      SerializeSpline( new XMLElement( *root, "AlignmentSplineX" ), m_S.m_Sx );
      SerializeSpline( new XMLElement( *root, "AlignmentSplineY" ), m_S.m_Sy );
   }

   if ( !m_location.IsEmpty() )
   {
      *(new XMLElement( *root, "LocationEstimates" )) << new XMLText( String().ToCommaSeparated( m_location ) );
      *(new XMLElement( *root, "ReferenceLocation" )) << new XMLText( String().ToCommaSeparated( m_referenceLocation ) );
      if ( !m_scale.IsEmpty() )
         *(new XMLElement( *root, "ScaleFactors" )) << new XMLText( String().ToCommaSeparated( m_scale ) );
      if ( !m_weight.IsEmpty() )
         *(new XMLElement( *root, "Weights" )) << new XMLText( String().ToCommaSeparated( m_weight ) );
      if ( !m_rejectHighData.IsEmpty() )
         SerializeRejectionData( new XMLElement( *root, "RejectionHigh" ), m_rejectHighData );
      if ( !m_rejectLowData.IsEmpty() )
         SerializeRejectionData( new XMLElement( *root, "RejectionLow" ), m_rejectLowData );
   }

   return xml.Release();
}

// ----------------------------------------------------------------------------

void DrizzleData::SerializeToFile( const String& path ) const
{
   AutoPointer<XMLDocument> xml = Serialize();
   xml->EnableAutoFormatting();
   xml->SetIndentSize( 3 );
   xml->SerializeToFile( path );
}

// ----------------------------------------------------------------------------

static bool TryToInt( int& value, IsoString::const_iterator p )
{
   IsoString::iterator endptr = nullptr;
   errno = 0;
   long val = ::strtol( p, &endptr, 0 );
   if ( errno == 0 && (endptr == nullptr || *endptr == '\0') )
   {
      value = int( val );
      return true;
   }
   return false;
}

static bool TryToDouble( double& value, IsoString::const_iterator p )
{
   IsoString::iterator endptr = nullptr;
   errno = 0;
   double val = ::strtod( p, &endptr );
   if ( errno == 0 && (endptr == nullptr || *endptr == '\0') )
   {
      value = val;
      return true;
   }
   return false;
}

static Vector ParseListOfRealValues( IsoString& text, size_type start, size_type end, size_type minCount = 0, size_type maxCount = ~size_type( 0 ) )
{
   Array<double> v;
   for ( size_type i = start, j; i < end; ++i )
   {
      for ( j = i; j < end; ++j )
         if ( text[j] == ',' )
            break;
      text[j] = '\0';
      double x;
      if ( !TryToDouble( x, text.At( i ) ) )
         throw Error( "Parsing real numeric list: Invalid floating point numeric literal \'" + IsoString( text.At( i ) ) + "\'" );
      if ( v.Length() == maxCount )
         throw Error( "Parsing real numeric list: Too many items." );
      v << x;
      i = j;
   }
   if ( v.Length() < minCount )
      throw Error( "Parsing real numeric list: Too few items." );
   return Vector( v.Begin(), int( v.Length() ) );
}

static Vector ParseListOfRealValues( const XMLElement& element, size_type minCount = 0, size_type maxCount = ~size_type( 0 ) )
{
   IsoString text = IsoString( element.Text().Trimmed() );
   return ParseListOfRealValues( text, 0, text.Length(), minCount, maxCount );
}

static IVector ParseListOfIntegerValues( IsoString& text, size_type start, size_type end, size_type minCount = 0, size_type maxCount = ~size_type( 0 ) )
{
   Array<int> v;
   for ( size_type i = start, j; i < end; ++i )
   {
      for ( j = i; j < end; ++j )
         if ( text[j] == ',' )
            break;
      text[j] = '\0';
      int x;
      if ( !TryToInt( x, text.At( i ) ) )
         throw Error( "Parsing integer numeric list: Invalid integer numeric literal \'" + IsoString( text.At( i ) ) + "\' at offset " + IsoString( start ) );
      if ( v.Length() == maxCount )
         throw Error( "Parsing integer numeric list: Too many items." );
      v << x;
      i = j;
   }
   if ( v.Length() < minCount )
      throw Error( "Parsing integer numeric list: Too few items." );
   return IVector( v.Begin(), int( v.Length() ) );
}

// static IVector ParseListOfIntegerValues( const XMLElement& element, size_type minCount = 0, size_type maxCount = ~size_type( 0 ) )
// {
//    IsoString text = IsoString( element.Text().Trimmed() );
//    return ParseListOfIntegerValues( text, 0, text.Length(), minCount, maxCount );
// }

static double ParseRealValue( const IsoString& s, size_type start, size_type end )
{
   double x;
   if ( !s.Substring( start, end-start ).TryToDouble( x ) )
      throw Error( "Invalid floating point numeric literal \'" + s + "\' at offset " + String( start ) );
   return x;
}

static int ParseIntegerValue( const IsoString& s, size_type start, size_type end )
{
   int x;
   if ( !s.Substring( start, end-start ).TryToInt( x ) )
      throw Error( "Invalid integer numeric literal \'" + s + "\' at offset " + IsoString( start ) );
   return x;
}

static DrizzleData::rejection_data ParseRejectionData( const XMLElement& element )
{
   IsoString text = IsoString( element.Text().Trimmed() );
   DrizzleData::rejection_data data;
   for ( size_type i = 0, j, n = text.Length(); i < n; ++i )
   {
      for ( j = i; j < n; ++j )
         if ( text[j] == ':' )
            break;

      IVector v = ParseListOfIntegerValues( text, i, j );
      if ( v.Length() % 2 )
         throw Error( "Parsing coordinate list: Insufficient items." );
      DrizzleData::rejection_coordinates points( v.Length() >> 1 );
      IVector::const_iterator p = v.Begin();
      for ( size_type i = 0; i < points.Length(); ++i )
      {
         points[i].x = *p++;
         points[i].y = *p++;
      }
      data << points;
      i = j;
   }
   return data;
}

// ----------------------------------------------------------------------------

void DrizzleData::Parse( const String& filePath, bool ignoreIntegrationData )
{
   IsoString text = File::ReadTextFile( filePath );
   for ( auto ch : text )
   {
      if ( ch == '<' )
      {
         XMLDocument xml;
         xml.SetParserOption( XMLParserOption::IgnoreComments );
         xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
         xml.Parse( text.UTF8ToUTF16() );
         Parse( xml, ignoreIntegrationData );
         return;
      }

      if ( !IsoCharTraits::IsSpace( ch ) )
      {
         PlainTextDecoder( this ).Decode( text );
         return;
      }
   }

   throw Error( "Empty drizzle data file." );
}

// ----------------------------------------------------------------------------

void DrizzleData::Parse( const XMLDocument& xml, bool ignoreIntegrationData )
{
   if ( xml.RootElement() == nullptr )
      throw Error( "The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xdrz" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XDRZ version 1.0 document." );
   Parse( *xml.RootElement(), ignoreIntegrationData );
}

// ----------------------------------------------------------------------------

void DrizzleData::Parse( const XMLElement& root, bool ignoreIntegrationData )
{
   Clear();

   for ( const XMLNode& node : root )
   {
      if ( !node.IsElement() )
      {
         if ( !node.IsComment() )
         {
            XMLParseError e( node,
                  "Parsing xdrz root element",
                  "Ignoring unexpected XML child node of " + XMLNodeType::AsString( node.NodeType() ) + " type." );
            Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
         }
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      try
      {
         if ( element.Name() == "SourceImage" )
         {
            m_sourceFilePath = element.Text().Trimmed();
            if ( m_sourceFilePath.IsEmpty() )
               throw Error( "Empty source file path definition." );
         }
         else if ( element.Name() == "CFASourceImage" )
         {
            // optional
            m_cfaSourceFilePath = element.Text().Trimmed();
            m_cfaSourcePattern = element.AttributeValue( "pattern" );
         }
         else if ( element.Name() == "AlignmentTargetImage" )
         {
            // optional
            m_alignTargetFilePath = element.Text().Trimmed();
         }
         else if ( element.Name() == "ReferenceGeometry" )
         {
            String width = element.AttributeValue( "width" );
            String height = element.AttributeValue( "height" );
            if ( width.IsEmpty() || height.IsEmpty() )
               throw Error( "Missing reference dimension attribute(s)." );
            m_referenceWidth = width.ToInt();
            m_referenceHeight = height.ToInt();
            if ( m_referenceWidth < 1 || m_referenceHeight < 1 )
               throw Error( "Invalid reference dimension(s)." );
         }
         else if ( element.Name() == "AlignmentMatrix" )
         {
            Vector v = ParseListOfRealValues( element, 9, 9 );
            m_H = Matrix( v.Begin(), 3, 3 );
         }
         else if ( element.Name() == "AlignmentSplineX" )
         {
            ParseSpline( m_Sx, element );
         }
         else if ( element.Name() == "AlignmentSplineY" )
         {
            ParseSpline( m_Sy, element );
         }
         else if ( element.Name() == "LocationEstimates" )
         {
            if ( !ignoreIntegrationData )
               m_location = ParseListOfRealValues( element, 1 );
         }
         else if ( element.Name() == "ReferenceLocation" )
         {
            if ( !ignoreIntegrationData )
               m_referenceLocation = ParseListOfRealValues( element, 1 );
         }
         else if ( element.Name() == "ScaleFactors" )
         {
            if ( !ignoreIntegrationData )
               m_scale = ParseListOfRealValues( element, 1 );
         }
         else if ( element.Name() == "Weights" )
         {
            if ( !ignoreIntegrationData )
               m_weight = ParseListOfRealValues( element, 1 );
         }
         else if ( element.Name() == "RejectionHigh" )
         {
            if ( !ignoreIntegrationData )
               m_rejectHighData = ParseRejectionData( element );
         }
         else if ( element.Name() == "RejectionLow" )
         {
            if ( !ignoreIntegrationData )
               m_rejectLowData = ParseRejectionData( element );
         }
         else if ( element.Name() == "CreationTime" )
         {
            m_creationTime = TimePoint( element.Text().Trimmed() );
         }
         else
         {
            XMLParseError e( node,
                  "Parsing xdrz root element",
                  "Skipping unknown \'" + element.Name() + "\' child element." );
            Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
         }
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }

   if ( m_sourceFilePath.IsEmpty() )
      throw Error( "Missing required SourceImage element." );

   if ( m_referenceWidth < 1 || m_referenceHeight < 1 )
      throw Error( "Missing required ReferenceGeometry element." );

   if ( m_H.IsEmpty() && !m_Sx.IsValid() )
      throw Error( "Missing required AlignmentMatrix or AlignmentSplineX/AlignmentSplineY element(s)." );

   if ( m_Sx.IsValid() != m_Sy.IsValid() )
      throw Error( "Missing required AlignmentSplineX/AlignmentSplineY element." );

   if ( !ignoreIntegrationData )
   {
      if ( m_location.IsEmpty() )
         throw Error( "Missing required LocationEstimates element." );

      if ( m_referenceLocation.IsEmpty() )
         throw Error( "Missing required ReferenceLocation element." );

      if ( m_location.Length() != m_referenceLocation.Length() )
         throw Error( "Incongruent reference location vector definition." );

      if ( m_location.Length() != m_scale.Length() )
         throw Error( "Incongruent scale factors vector definition." );

      if ( m_location.Length() != m_weight.Length() )
         throw Error( "Incongruent image weights vector definition." );

      if ( !m_rejectHighData.IsEmpty() )
      {
         if ( size_type( m_location.Length() ) != m_rejectHighData.Length() )
            throw Error( "Incongruent high pixel rejection vector definition." );
         m_rejectionHighCount = UI64Vector( uint64( 0 ), m_location.Length() );
         for ( int i = 0; i < m_location.Length(); ++i )
            m_rejectionHighCount[i] = m_rejectHighData[i].Length();
      }

      if ( !m_rejectLowData.IsEmpty() )
      {
         if ( size_type( m_location.Length() ) != m_rejectLowData.Length() )
            throw Error( "Incongruent low pixel rejection vector definition." );
         m_rejectionLowCount = UI64Vector( uint64( 0 ), m_location.Length() );
         for ( int i = 0; i < m_location.Length(); ++i )
            m_rejectionLowCount[i] = m_rejectLowData[i].Length();
      }
   }

   if ( m_Sx.IsValid() )
   {
      m_S.m_Sx = m_Sx;
      m_S.m_Sy = m_Sy;
      m_Sx.Clear();
      m_Sy.Clear();
   }
}

// ----------------------------------------------------------------------------

void DrizzleData::ParseSpline( DrizzleData::spline& S, const XMLElement& root )
{
   // Scaling factor for normalization of node coordinates
   String s = root.AttributeValue( "scalingFactor" );
   if ( s.IsEmpty() )
      throw Error( "Missing surface spline scalingFactor attribute." );
   S.m_r0 = s.ToDouble();
   if ( S.m_r0 <= 0 )
      throw Error( "Invalid surface spline scaling factor '" + s + '\'' );

   // Zero offset for normalization of X node coordinates
   s = root.AttributeValue( "zeroOffsetX" );
   if ( s.IsEmpty() )
      throw Error( "Missing surface spline zeroOffsetX attribute." );
   S.m_x0 = s.ToDouble();

   // Zero offset for normalization of Y node coordinates
   s = root.AttributeValue( "zeroOffsetY" );
   if ( s.IsEmpty() )
      throw Error( "Missing surface spline zeroOffsetY attribute." );
   S.m_y0 = s.ToDouble();

   // Derivative order > 0
   s = root.AttributeValue( "order" );
   if ( s.IsEmpty() )
      throw Error( "Missing surface spline order attribute." );
   S.m_order = s.ToInt();
   if ( S.m_order < 1 )
      throw Error( "Invalid surface spline derivative order '" + s + '\'' );

   // Smoothing factor, or interpolating 2-D spline if m_smoothing == 0
   s = root.AttributeValue( "smoothing" );
   if ( !s.IsEmpty() )
   {
      S.m_smoothing = s.ToFloat();
      if ( S.m_smoothing < 0 )
         throw Error( "Invalid surface spline smoothing factor '" + s + '\'' );
   }
   else
      S.m_smoothing = 0;

   S.m_x.Clear();
   S.m_y.Clear();
   S.m_weights.Clear();
   S.m_spline.Clear();

   for ( const XMLNode& node : root )
   {
      if ( !node.IsElement() )
      {
         if ( !node.IsComment() )
         {
            XMLParseError e( node,
                  "Parsing xdrz AlignmentSplineX/AlignmentSplineY element",
                  "Ignoring unexpected XML child node of " + XMLNodeType::AsString( node.NodeType() ) + " type." );
            Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
         }
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      if ( element.Name() == "NodeXCoordinates" )
      {
         S.m_x = ParseListOfRealValues( element, 3 );
      }
      else if ( element.Name() == "NodeYCoordinates" )
      {
         S.m_y = ParseListOfRealValues( element, 3 );
      }
      else if ( element.Name() == "Coefficients" )
      {
         S.m_spline = ParseListOfRealValues( element, 3 );
      }
      else if ( element.Name() == "NodeWeights" )
      {
         S.m_weights = ParseListOfRealValues( element );
      }
      else
      {
         XMLParseError e( node,
               "Parsing xdrz AlignmentSplineX/AlignmentSplineY element",
               "Skipping unknown \'" + element.Name() + "\' child element." );
         Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
      }
   }

   if ( S.m_x.Length() < 3 )
      throw Error( "Missing surface spline NodeXCoordinates child element." );
   if ( S.m_y.Length() < 3 )
      throw Error( "Missing surface spline NodeYCoordinates child element." );
   if ( S.m_spline.Length() < 3 )
      throw Error( "Missing surface spline Coefficients child element." );

   if ( S.m_x.Length() != S.m_y.Length() ||
       !S.m_weights.IsEmpty() && S.m_weights.Length() != S.m_x.Length() ||
        S.m_spline.Length() != S.m_x.Length() + ((S.m_order*(S.m_order + 1)) >> 1) )
   {
      throw Error( "Invalid surface spline definition." );
   }
}

// ----------------------------------------------------------------------------

void DrizzleData::MakeRejectionMap() const
{
   m_rejectionMap.FreeData();

   if ( !m_rejectHighData.IsEmpty() || !m_rejectLowData.IsEmpty() )
   {
      m_rejectionMap.AllocateData( m_referenceWidth, m_referenceHeight, NumberOfChannels() );
      m_rejectionMap.Zero();

      if ( !m_rejectHighData.IsEmpty() )
         for ( int c = 0; c < NumberOfChannels(); ++c )
            for ( const Point& p : m_rejectHighData[c] )
               m_rejectionMap( p, c ) = uint8( 1 );

      if ( !m_rejectLowData.IsEmpty() )
         for ( int c = 0; c < NumberOfChannels(); ++c )
            for ( const Point& p : m_rejectLowData[c] )
               m_rejectionMap( p, c ) |= uint8( 2 );
   }
}

// ----------------------------------------------------------------------------

void DrizzleData::SerializeSpline( XMLElement* root, const DrizzleData::spline& S )
{
   root->SetAttribute( "scalingFactor", String( S.m_r0 ) );
   root->SetAttribute( "zeroOffsetX", String( S.m_x0 ) );
   root->SetAttribute( "zeroOffsetY", String( S.m_y0 ) );
   root->SetAttribute( "order", String( S.m_order ) );
   *(new XMLElement( *root, "NodeXCoordinates" )) << new XMLText( String().ToCommaSeparated( S.m_x ) );
   *(new XMLElement( *root, "NodeYCoordinates" )) << new XMLText( String().ToCommaSeparated( S.m_y ) );
   *(new XMLElement( *root, "Coefficients" ))     << new XMLText( String().ToCommaSeparated( S.m_spline ) );
   if ( S.m_smoothing > 0 )
   {
      root->SetAttribute( "smoothing", String( S.m_smoothing ) );
      if ( !S.m_weights.IsEmpty() )
         (*new XMLElement( *root, "NodeWeights" )) << new XMLText( String().ToCommaSeparated( S.m_weights ) );
   }
}

// ----------------------------------------------------------------------------

void DrizzleData::SerializeRejectionData( XMLElement* root, const DrizzleData::rejection_data& R )
{
   String text;
   for ( size_type i = 0; ; )
   {
      if ( !R[i].IsEmpty() )
         for ( size_type j = 0; ; )
         {
            text.AppendFormat( "%d,%d", R[i][j].x, R[i][j].y );
            if ( ++j == R[i].Length() )
               break;
            text << ',';
         }
      if ( ++i == R.Length() )
         break;
      text << ':';
   }
   *root << new XMLText( text );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DrizzleData::PlainTextDecoder::Decode( IsoString& s, size_type start, size_type end )
{
   if ( end <= start )
      end = s.Length();
   IsoString itemId;
   size_type block = 0;
   size_type blockStart = 0;
   for ( size_type i = start; i < end; ++i )
      switch ( s[i] )
      {
      case '{':
         if ( block++ == 0 )
         {
            blockStart = i;
            itemId.Trim();
            if ( itemId.IsEmpty() )
               throw Error( "At offset=" + String( i ) + ": Missing item identifier." );
         }
         break;
      case '}':
         if ( --block < 0 )
            throw Error( "At offset=" + String( i ) + ": Unexpected block termination." );
         if ( block == 0 )
         {
            ProcessBlock( s, itemId, blockStart+1, i );
            itemId.Clear();
         }
         break;
      default:
         if ( block == 0 )
            itemId << s[i];
         break;
      }

   if ( block > 0 )
      throw Error( "At offset=" + String( blockStart ) + ": Unterminated block." );
   if ( !itemId.IsEmpty() )
      throw Error( "Uncompleted item definition \'" + itemId + '\'' );
}

// ----------------------------------------------------------------------------

void DrizzleData::PlainTextDecoder::ProcessBlock( IsoString& s, const IsoString& itemId, size_type start, size_type end )
{
   if ( itemId == "P" ) // drizzle source image
   {
      m_data->m_sourceFilePath = s.Substring( start, end-start ).Trimmed().UTF8ToUTF16();
      if ( m_data->m_sourceFilePath.IsEmpty() )
         throw Error( "At offset=" + String( start ) + ": Empty file path defined." );
   }
   else if ( itemId == "T" ) // alignment target image (optional)
   {
      m_data->m_alignTargetFilePath = s.Substring( start, end-start ).Trimmed().UTF8ToUTF16();
      if ( m_data->m_alignTargetFilePath.IsEmpty() )
         throw Error( "At offset=" + String( start ) + ": Empty file path defined." );
   }
   else if ( itemId == "D" ) // alignment reference image dimensions
   {
      IVector v = ParseListOfIntegerValues( s, start, end, 2, 2 );
      m_data->m_referenceWidth = v[0];
      m_data->m_referenceHeight = v[1];
      if ( m_data->m_referenceWidth < 1 || m_data->m_referenceHeight < 1 )
         throw Error( "At offset=" + String( start ) + ": Invalid reference dimensions." );
   }
   else if ( itemId == "H" ) // alignment matrix (projective)
   {
      Vector v = ParseListOfRealValues( s, start, end, 9, 9 );
      m_data->m_H = Matrix( v.Begin(), 3, 3 );
   }
   else if ( itemId == "Sx" ) // registration thin plates, X-axis
      m_data->m_Sx = ParseSurfaceSpline( s, start, end );
   else if ( itemId == "Sy" ) // registration thin plates, Y-axis
      m_data->m_Sy = ParseSurfaceSpline( s, start, end );
   else if ( itemId == "m" ) // location vector
      m_data->m_location = ParseListOfRealValues( s, start, end, 1 );
   else if ( itemId == "m0" ) // reference location vector
      m_data->m_referenceLocation = ParseListOfRealValues( s, start, end, 1 );
   else if ( itemId == "s" ) // scaling factors vector
      m_data->m_scale = ParseListOfRealValues( s, start, end, 1 );
   else if ( itemId == "w" ) // image weights vector
      m_data->m_weight = ParseListOfRealValues( s, start, end, 1 );
   else if ( itemId == "Rl" ) // rejection pixel coordinates, low values
      m_data->m_rejectLowData = ParseRejectionData( s, start, end );
   else if ( itemId == "Rh" ) // rejection pixel coordinates, high values
      m_data->m_rejectHighData = ParseRejectionData( s, start, end );
   else
      throw Error( "At offset=" + String( start ) + ": Unknown item identifier \'" + itemId + '\'' );
}

// ----------------------------------------------------------------------------

DrizzleData::rejection_coordinates
DrizzleData::PlainTextDecoder::ParseRejectionCoordinates( IsoString& s, size_type start, size_type end )
{
   IVector v = ParseListOfIntegerValues( s, start, end );
   if ( v.Length() & 1 )
      throw Error( "Parsing list from offset=" + String( start ) + ": Missing point coordinate(s)." );
   rejection_coordinates P;
   for ( int i = 0; i < v.Length(); i += 2 )
      P << Point( v[i], v[i+1] );
   return P;
}

// ----------------------------------------------------------------------------

DrizzleData::rejection_data
DrizzleData::PlainTextDecoder::ParseRejectionData( IsoString& s, size_type start, size_type end )
{
   rejection_data R;
   for ( size_type i = start; i < end; ++i )
      if ( s[i] == '{' )
      {
         size_type j = s.Find( '}', ++i );
         if ( j >= end )
            throw Error( "At offset=" + String( i ) + ": Unterminated block." );
         R << ParseRejectionCoordinates( s, i, j );
         i = j;
      }
      else if ( !IsoCharTraits::IsSpace( s[i] ) )
         throw Error( "At offset=" + String( i ) + ": Unexpected token \'" + s[i] + '\'' );
   return R;
}

// ----------------------------------------------------------------------------

DrizzleData::spline
DrizzleData::PlainTextDecoder::ParseSurfaceSpline( IsoString& text, size_type start, size_type end )
{
   spline S;
   PlainTextSplineDecoder( S ).Decode( text, start, end );
   return S;
}

// ----------------------------------------------------------------------------

void DrizzleData::PlainTextSplineDecoder::ProcessBlock( IsoString& s, const IsoString& itemId, size_type start, size_type end )
{
   if ( itemId == "x" )
      m_S.m_x = ParseListOfRealValues( s, start, end, 3 );
   else if ( itemId == "y" )
      m_S.m_y = ParseListOfRealValues( s, start, end, 3 );
   else if ( itemId == "r0" )
      m_S.m_r0 = ParseRealValue( s, start, end );
   else if ( itemId == "x0" )
      m_S.m_x0 = ParseRealValue( s, start, end );
   else if ( itemId == "y0" )
      m_S.m_y0 = ParseRealValue( s, start, end );
   else if ( itemId == "m" )
      m_S.m_order = ParseIntegerValue( s, start, end );
   else if ( itemId == "r" )
      m_S.m_smoothing = ParseRealValue( s, start, end );
   else if ( itemId == "w" )
      m_S.m_weights = ParseListOfRealValues( s, start, end );
   else if ( itemId == "s" )
      m_S.m_spline = ParseListOfRealValues( s, start, end, 3 );
   else
      throw Error( "At offset=" + String( start ) + ": Unknown item identifier \'" + itemId + '\'' );
}

// ----------------------------------------------------------------------------

} // pcl
