//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/DrizzleData.cpp - Released 2017-06-28T11:58:42Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/AutoPointer.h>
#include <pcl/Compression.h>
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
   m_rejectionMap.FreeData();
   m_rejectLowData = m_rejectHighData = rejection_data();
}

// ----------------------------------------------------------------------------

XMLDocument* DrizzleData::Serialize() const
{
   // Validate image registration data
   if ( m_sourceFilePath.IsEmpty() ||
        m_referenceWidth < 1 || m_referenceHeight < 1 ||
       !m_H.IsEmpty() && (m_H.Rows() != 3 || m_H.Columns() != 3) ||
        m_H.IsEmpty() && !m_S.IsValid() )
      throw Error( "Invalid or insufficient image registration data." );

   // Validate image integration data
   if ( m_location.Length() != m_referenceLocation.Length() ||
       !m_scale.IsEmpty() && m_location.Length() != m_scale.Length() ||
       !m_weight.IsEmpty() && m_location.Length() != m_weight.Length() ||
       !m_rejectionMap.IsEmpty() && m_location.Length() != m_rejectionMap.NumberOfChannels() )
      throw Error( "Invalid or insufficient image integration data." );

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
      if ( !m_rejectionMap.IsEmpty() )
         SerializeRejectionMap( new XMLElement( *root, "RejectionMap" ) );
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

static void WarnOnUnexpectedChildNode( const XMLNode& node, const String& parsingWhatElement )
{
   if ( !node.IsComment() )
   {
      XMLParseError e( node,
            "Parsing " + parsingWhatElement + " element",
            "Ignoring unexpected XML child node of " + XMLNodeType::AsString( node.NodeType() ) + " type." );
      Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
   }
}

static void WarnOnUnknownChildElement( const XMLElement& element, const String& parsingWhatElement )
{
   XMLParseError e( element,
         "Parsing " + parsingWhatElement + " element",
         "Skipping unknown \'" + element.Name() + "\' child element." );
   Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
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

// ----------------------------------------------------------------------------

template <typename T>
static GenericVector<T> ParseBase64EncodedVector( const XMLElement& element, size_type minCount = 0, size_type maxCount = ~size_type( 0 ) )
{
   ByteArray data = IsoString( element.Text().Trimmed() ).FromBase64();
   if ( data.IsEmpty() )
      throw Error( "Missing encoded vector data in " + element.Name() + " element." );
   if ( data.Size() % sizeof( T ) != 0 )
      throw Error( "Invalid size of encoded vector data in " + element.Name() + " element." );
   size_type n = data.Size()/sizeof( T );
   if ( n < minCount )
      throw Error( "Too few vector components in " + element.Name() + " element." );
   if ( n > maxCount )
      throw Error( "Too many vector components in " + element.Name() + " element." );
   return GenericVector<T>( reinterpret_cast<const T*>( data.Begin() ), int( n ) );
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
         Clear();
         PlainTextDecoder( this, ignoreIntegrationData ).Decode( text );

         // Build rejection map from rejection coordinate lists.
         if ( !m_rejectHighData.IsEmpty() || !m_rejectLowData.IsEmpty() )
         {
            m_rejectionMap.AllocateData( m_referenceWidth, m_referenceHeight, NumberOfChannels() );
            m_rejectionMap.Zero();

            if ( !m_rejectHighData.IsEmpty() )
            {
               for ( int c = 0; c < NumberOfChannels(); ++c )
                  for ( const Point& p : m_rejectHighData[c] )
                     m_rejectionMap( p, c ) = uint8( 1 );
               m_rejectHighData.Clear();
            }

            if ( !m_rejectLowData.IsEmpty() )
            {
               for ( int c = 0; c < NumberOfChannels(); ++c )
                  for ( const Point& p : m_rejectLowData[c] )
                     m_rejectionMap( p, c ) |= uint8( 2 );
               m_rejectLowData.Clear();
            }
         }

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
         WarnOnUnexpectedChildNode( node, "xdrz root" );
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
         else if ( element.Name() == "RejectionMap" )
         {
            if ( !ignoreIntegrationData )
               ParseRejectionMap( element );
         }
         else if ( element.Name() == "CreationTime" )
         {
            m_creationTime = TimePoint( element.Text().Trimmed() );
         }
         else
         {
            WarnOnUnknownChildElement( element, "xdrz root" );
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

      if ( !m_scale.IsEmpty() )
         if ( m_location.Length() != m_scale.Length() )
            throw Error( "Incongruent scale factors vector definition." );

      if ( !m_weight.IsEmpty() )
         if ( m_location.Length() != m_weight.Length() )
            throw Error( "Incongruent image weights vector definition." );

      if ( !m_rejectionMap.IsEmpty() )
      {
         if ( m_location.Length() != m_rejectionMap.NumberOfChannels() )
            throw Error( "Incongruent pixel rejection map definition." );
         m_rejectionHighCount = UI64Vector( uint64( 0 ), m_location.Length() );
         m_rejectionLowCount = UI64Vector( uint64( 0 ), m_location.Length() );
         for ( UInt8Image::const_pixel_iterator i( m_rejectionMap ); i; ++i )
            for ( int j = 0; j < m_location.Length(); ++j )
            {
               if ( i[j] & 1 )
                  ++m_rejectionHighCount[j];
               if ( i[j] & 2 )
                  ++m_rejectionLowCount[j];
            }
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

void DrizzleData::ParseRejectionMap( const XMLElement& root )
{
   String s = root.AttributeValue( "width" );
   if ( s.IsEmpty() )
      throw Error( "Missing rejection map width attribute." );
   int width = s.ToInt();
   if ( width < 1 )
      throw Error( "Invalid rejection map width attribute value '" + s + '\'' );

   s = root.AttributeValue( "height" );
   if ( s.IsEmpty() )
      throw Error( "Missing rejection map height attribute." );
   int height = s.ToInt();
   if ( height < 1 )
      throw Error( "Invalid rejection map height attribute value '" + s + '\'' );

   s = root.AttributeValue( "numberOfChannels" );
   if ( s.IsEmpty() )
      throw Error( "Missing rejection map numberOfChannels attribute." );
   int numberOfChannels = s.ToInt();
   if ( numberOfChannels < 1 )
      throw Error( "Invalid rejection map numberOfChannels attribute value '" + s + '\'' );

   m_rejectionMap.AllocateData( width, height, numberOfChannels );

   int channel = 0;

   for ( const XMLNode& node : root )
   {
      if ( !node.IsElement() )
      {
         WarnOnUnexpectedChildNode( node, "RejectionMap" );
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      if ( element.Name() == "ChannelData" )
      {
         if ( channel == numberOfChannels )
            throw Error( "Unexpected ChannelData child element - all rejection map channels are already defined." );

         ByteArray channelData;

         String compressionName = element.AttributeValue( "compression" ).CaseFolded();
         if ( !compressionName.IsEmpty() )
         {
            AutoPointer<Compression> compression;
            if ( compressionName == "lz4" )
               compression = new LZ4Compression;
            else if ( compressionName == "lz4hc" )
               compression = new LZ4HCCompression;
            else if ( compressionName == "zlib" )
               compression = new ZLibCompression;
            else
               throw Error( "Unknown or unsupported compression codec '" + compressionName + '\'' );

            Compression::subblock_list subblocks;

            for ( const XMLNode& node : element )
            {
               if ( !node.IsElement() )
               {
                  WarnOnUnexpectedChildNode( node, "ChannelData" );
                  continue;
               }

               const XMLElement& subElement = static_cast<const XMLElement&>( node );

               if ( subElement.Name() == "Subblock" )
               {
                  Compression::Subblock subblock;
                  s = subElement.AttributeValue( "uncompressedSize" );
                  if ( s.IsEmpty() )
                     throw Error( "Missing subblock uncompressedSize attribute." );
                  subblock.uncompressedSize = s.ToUInt64();
                  subblock.compressedData = IsoString( subElement.Text().Trimmed() ).FromBase64();
                  subblocks << subblock;
               }
               else
               {
                  WarnOnUnknownChildElement( element, "ChannelData" );
               }
            }

            if ( subblocks.IsEmpty() )
               throw Error( "Parsing xdrz RejectionMap ChannelData element: Missing Subblock child element(s)." );

            channelData = compression->Uncompress( subblocks );
         }
         else
         {
            channelData = IsoString( element.Text().Trimmed() ).FromBase64();
         }

         if ( channelData.Size() != m_rejectionMap.ChannelSize() )
            throw Error( "Parsing xdrz RejectionMap ChannelData element: Invalid channel data size: "
               "Expected " + String( m_rejectionMap.ChannelSize() ) + " bytes, "
               "got " + String( channelData.Size() ) + " bytes." );

         ::memcpy( m_rejectionMap[channel], channelData.Begin(), channelData.Size() );

         ++channel;
      }
      else
      {
         WarnOnUnknownChildElement( element, "RejectionMap" );
      }
   }

   if ( channel < numberOfChannels )
      throw Error( "Missing rejection map channel data." );
}

// ----------------------------------------------------------------------------

void DrizzleData::SerializeRejectionMap( XMLElement* root ) const
{
   root->SetAttribute( "width", String( m_rejectionMap.Width() ) );
   root->SetAttribute( "height", String( m_rejectionMap.Height() ) );
   root->SetAttribute( "numberOfChannels", String( m_rejectionMap.NumberOfChannels() ) );

   for ( int c = 0; c < m_rejectionMap.NumberOfChannels(); ++c )
   {
      XMLElement* element = new XMLElement( *root, "ChannelData" );
      if ( m_compressionEnabled )
      {
         LZ4Compression compression;
         Compression::subblock_list subblocks = compression.Compress( m_rejectionMap[c], m_rejectionMap.ChannelSize() );
         if ( !subblocks.IsEmpty() )
         {
            element->SetAttribute( "compression", compression.AlgorithmName() );
            for ( const Compression::Subblock& subblock : subblocks )
            {
               XMLElement* subblockElement = new XMLElement( *element, "Subblock" );
               subblockElement->SetAttribute( "uncompressedSize", String( subblock.uncompressedSize ) );
               *subblockElement << new XMLText( IsoString::ToBase64( subblock.compressedData ) );
            }
            continue;
         }
      }

      *element << new XMLText( IsoString::ToBase64( m_rejectionMap[c], m_rejectionMap.ChannelSize() ) );
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
         WarnOnUnexpectedChildNode( node, "AlignmentSplineX/AlignmentSplineY" );
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      if ( element.Name() == "NodeXCoordinates" )
      {
         S.m_x = ParseBase64EncodedVector<vector_spline::spline::scalar>( element, 3 );
      }
      else if ( element.Name() == "NodeYCoordinates" )
      {
         S.m_y = ParseBase64EncodedVector<vector_spline::spline::scalar>( element, 3 );
      }
      else if ( element.Name() == "Coefficients" )
      {
         S.m_spline = ParseBase64EncodedVector<vector_spline::spline::scalar>( element, 3 );
      }
      else if ( element.Name() == "NodeWeights" )
      {
         S.m_weights = ParseBase64EncodedVector<FVector::scalar>( element, 3 );
      }
      else
      {
         WarnOnUnknownChildElement( element, "AlignmentSplineX/AlignmentSplineY" );
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

void DrizzleData::SerializeSpline( XMLElement* root, const DrizzleData::spline& S )
{
   root->SetAttribute( "scalingFactor", String( S.m_r0 ) );
   root->SetAttribute( "zeroOffsetX", String( S.m_x0 ) );
   root->SetAttribute( "zeroOffsetY", String( S.m_y0 ) );
   root->SetAttribute( "order", String( S.m_order ) );
   *(new XMLElement( *root, "NodeXCoordinates" )) << new XMLText( IsoString::ToBase64( S.m_x ) );
   *(new XMLElement( *root, "NodeYCoordinates" )) << new XMLText( IsoString::ToBase64( S.m_y ) );
   *(new XMLElement( *root, "Coefficients" ))     << new XMLText( IsoString::ToBase64( S.m_spline ) );
   if ( S.m_smoothing > 0 )
   {
      root->SetAttribute( "smoothing", String( S.m_smoothing ) );
      if ( !S.m_weights.IsEmpty() )
         (*new XMLElement( *root, "NodeWeights" )) << new XMLText( IsoString::ToBase64( S.m_weights ) );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Compatibility with the old .drz plain text format.
 */

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
         if ( block == 0 )
            throw Error( "At offset=" + String( i ) + ": Unexpected block termination." );
         if ( --block == 0 )
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
   {
      m_data->m_Sx = ParseSurfaceSpline( s, start, end );
   }
   else if ( itemId == "Sy" ) // registration thin plates, Y-axis
   {
      m_data->m_Sy = ParseSurfaceSpline( s, start, end );
   }
   else if ( itemId == "m" ) // location vector
   {
      if ( !m_ignoreIntegrationData )
         m_data->m_location = ParseListOfRealValues( s, start, end, 1 );
   }
   else if ( itemId == "m0" ) // reference location vector
   {
      if ( !m_ignoreIntegrationData )
         m_data->m_referenceLocation = ParseListOfRealValues( s, start, end, 1 );
   }
   else if ( itemId == "s" ) // scaling factors vector
   {
      if ( !m_ignoreIntegrationData )
         m_data->m_scale = ParseListOfRealValues( s, start, end, 1 );
   }
   else if ( itemId == "w" ) // image weights vector
   {
      if ( !m_ignoreIntegrationData )
         m_data->m_weight = ParseListOfRealValues( s, start, end, 1 );
   }
   else if ( itemId == "Rl" ) // rejection pixel coordinates, low values
   {
      if ( !m_ignoreIntegrationData )
         m_data->m_rejectLowData = ParseRejectionData( s, start, end );
   }
   else if ( itemId == "Rh" ) // rejection pixel coordinates, high values
   {
      if ( !m_ignoreIntegrationData )
         m_data->m_rejectHighData = ParseRejectionData( s, start, end );
   }
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

// ----------------------------------------------------------------------------
// EOF pcl/DrizzleData.cpp - Released 2017-06-28T11:58:42Z
