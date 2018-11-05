//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/EphemerisFile.cpp - Released 2018-11-01T11:06:51Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/AutoLock.h>
#include <pcl/Console.h>
#include <pcl/EphemerisFile.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Version.h>
#include <pcl/XML.h>

#include <iostream>

namespace pcl
{

// ----------------------------------------------------------------------------

String EphemerisFile::s_ephFilePath;
String EphemerisFile::s_ephFilePath_s;
String EphemerisFile::s_astFilePath;
String EphemerisFile::s_astFilePath_s;
String EphemerisFile::s_nutFilePath;
String EphemerisFile::s_nutFilePath_s;
String EphemerisFile::s_deltaTFilePath;
String EphemerisFile::s_deltaATFilePath;
String EphemerisFile::s_cipITRSFilePath;

// ----------------------------------------------------------------------------

static EphemerisFile* s_E = nullptr;
static EphemerisFile* s_Es = nullptr;
static EphemerisFile* s_A = nullptr;
static EphemerisFile* s_As = nullptr;
static EphemerisFile* s_N = nullptr;
static EphemerisFile* s_Ns = nullptr;

static Mutex s_mutex;

// ----------------------------------------------------------------------------

struct XEPHFileSignature
{
   uint8  magic[ 8 ]   = { 'X', 'E', 'P', 'H', '0', '1', '0', '0' };
   uint32 headerLength = 0;  // length in bytes of the XML file header
   uint32 reserved     = 0;  // reserved - must be zero

   XEPHFileSignature() = default;

   XEPHFileSignature( const XEPHFileSignature& ) = default;

   XEPHFileSignature& operator =( const XEPHFileSignature& ) = default;

   XEPHFileSignature( uint32 length ) : headerLength( length )
   {
   }

   void Validate() const
   {
      if ( magic[0] != 'X' || magic[1] != 'E' || magic[2] != 'P' || magic[3] != 'H' )
         throw Error( "Not an XEPH file." );
      if ( magic[4] != '0' || magic[5] != '1' || magic[6] != '0' || magic[7] != '0' )
         throw Error( "Not an XEPH version 1.0 file." );
      if ( headerLength < 65 ) // minimum length of an empty XEPH header, from "<?xml..." to </xeph>
         throw Error( "Invalid or corrupted XEPH file." );
   }
};

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

void EphemerisFile::Open( const String& filePath )
{
   Close();

   volatile AutoLock lock( m_mutex );

   m_file.OpenForReading( filePath );

   XMLDocument xml;
   fsize_type fileSize, minPos;
   {
      XEPHFileSignature signature;
      m_file.Read( signature );
      signature.Validate();

      fileSize = m_file.Size();
      minPos = signature.headerLength + sizeof( XEPHFileSignature );

      IsoString header;
      header.SetLength( signature.headerLength );
      m_file.Read( reinterpret_cast<void*>( header.Begin() ), signature.headerLength );

      xml.SetParserOption( XMLParserOption::IgnoreComments );
      xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
      xml.Parse( header.UTF8ToUTF16() );
   }

   if ( xml.RootElement()->Name() != "xeph" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XEPH version 1.0 file." );

   for ( const XMLNode& node : *xml.RootElement() )
   {
      if ( !node.IsElement() )
      {
         WarnOnUnexpectedChildNode( node, "xeph root" );
         continue;
      }

      const XMLElement& element = static_cast<const XMLElement&>( node );

      try
      {
         if ( element.Name() == "Object" )
         {
            String id = element.AttributeValue( "id" );
            String origin = element.AttributeValue( "origin" );
            String name = element.AttributeValue( "name" );

            if ( id.IsEmpty() )
               throw Error( "Missing object id attribute." );
            if ( origin.IsEmpty() )
               throw Error( "Missing object origin attribute." );
            if ( origin == id )
               throw Error( "The object and origin identifiers must be different." );

            Index index( id.ToIsoString(), origin.ToIsoString(), name );
            if ( m_index.Contains( index ) )
               throw Error( "Duplicate object '" + id + "' with origin '" + origin + '\'' );

            for ( const XMLNode& node : element )
            {
               if ( !node.IsElement() )
               {
                  WarnOnUnexpectedChildNode( node, "Object" );
                  continue;
               }

               const XMLElement& element = static_cast<const XMLElement&>( node );
               if ( element.Name() == "Index" )
               {
                  String sOrder = element.AttributeValue( "order" );
                  String sNumberOfExpansions = element.AttributeValue( "numberOfExpansions" );
                  String sPosition = element.AttributeValue( "position" );

                  int order = 0;
                  if ( !sOrder.IsEmpty() )
                  {
                     order = sOrder.ToInt();
                     if ( order < 0 || order > 1 )
                        throw Error( "Invalid or unsupported index order attribute value." );
                  }

                  if ( !index.nodes[order].IsEmpty() )
                     throw Error( "Duplicate index definition for derivative order " + String( order )  + '.' );

                  if ( sNumberOfExpansions.IsEmpty() )
                     throw Error( "Missing index numberOfExpansions attribute." );
                  if ( sPosition.IsEmpty() )
                     throw Error( "Missing index position attribute." );

                  int numberOfExpansions = sNumberOfExpansions.ToInt();
                  if ( numberOfExpansions < 1 )
                     throw Error( "Invalid index numberOfExpansions attribute value." );

                  fpos_type position = sPosition.ToInt64();
                  if ( position < minPos || position >= fileSize )
                     throw Error( "Invalid index position attribute value." );

                  m_file.SetPosition( position );
                  for ( int i = 0; i < numberOfExpansions; ++i )
                  {
                     IndexNode node;
                     m_file.Read( node );
                     index.nodes[order] << node;
                  }
               }
               else if ( element.Name() == "Description" )
               {
                  index.objectDescription = element.Text().Trimmed();
               }
               else
                  WarnOnUnknownChildElement( element, "Object" );
            }

            m_index << index;
         }
         else if ( element.Name() == "TimeSpan" )
         {
            String start = element.AttributeValue( "start" );
            String end = element.AttributeValue( "end" );
            if ( start.IsEmpty() )
               throw Error( "Missing time span start attribute." );
            if ( end.IsEmpty() )
               throw Error( "Missing time span end attribute." );
            m_startTime = TimePoint( start );
            m_endTime = TimePoint( end );
            if ( m_endTime < m_startTime )
               Swap( m_startTime, m_endTime );
            if ( 1 + (m_endTime - m_startTime) == 1 )
               throw Error( "Empty or insignificant time span defined." );
         }
         else if ( element.Name() == "Constants" )
         {
            IsoString text( element.Text().Trimmed() );
            IsoStringList items;
            text.Break( items, ',', true/*trim*/ );
            for ( const IsoString& item : items )
            {
               IsoStringList tokens;
               item.Break( tokens, '=', true/*trim*/ );
               if ( tokens.Length() != 2 )
                  throw Error( "Invalid ephemeris constant specification '" + item + "'." );
               if ( tokens[0].IsEmpty() )
                  throw Error( "Empty ephemeris constant name: '" + item + "'." );
               if ( tokens[1].IsEmpty() )
                  throw Error( "Missing value of ephemeris constant '" + tokens[0] + "'." );
               if ( m_constants.Contains( tokens[0] ) )
                  throw Error( "Duplicate ephemeris constant specification '" + tokens[0] + "'." );
               m_constants << EphemerisConstant( tokens[0], tokens[1].ToDouble() );
            }
         }
         else if ( element.Name() == "Metadata" )
         {
            for ( const XMLNode& node : element )
            {
               if ( !node.IsElement() )
               {
                  WarnOnUnexpectedChildNode( node, "Metadata" );
                  continue;
               }

               const XMLElement& element = static_cast<const XMLElement&>( node );
               String text = element.Text().Trimmed();
               if ( element.Name() == "CreationTime" )
                  m_metadata.creationTime = TimePoint( text );
               else if ( element.Name() == "CreatorOS" )
                  m_metadata.creatorOS = text;
               else if ( element.Name() == "CreatorApplication" )
                  m_metadata.creatorApplication = text;
               else if ( element.Name() == "Title" )
                  m_metadata.title = text;
               else if ( element.Name() == "BriefDescription" )
                  m_metadata.briefDescription = text;
               else if ( element.Name() == "Description" )
                  m_metadata.description = text;
               else if ( element.Name() == "OrganizationName" )
                  m_metadata.organizationName = text;
               else if ( element.Name() == "Authors" )
                  m_metadata.authors = text;
               else if ( element.Name() == "Copyright" )
                  m_metadata.copyright = text;
               else
                  WarnOnUnknownChildElement( element, "Metadata" );
            }
         }
         else
         {
            WarnOnUnknownChildElement( element, "xeph root" );
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

   if ( !m_startTime.IsValid() || !m_endTime.IsValid() )
      throw Error( "Missing required TimeSpan element." );

   if ( m_index.IsEmpty() )
      throw Error( "No ephemeris data available." );

   for ( const Index& ix : m_index )
      for ( int order = 0; order < 2; ++order )
      {
         try
         {
            const Array<IndexNode> nodes = ix.nodes[order];
            if ( nodes.IsEmpty() )
            {
               if ( order > 0 )
                  break;
               throw Error( "Empty expansion data" );
            }

            if ( nodes[0].StartTime() != m_startTime )
               throw Error( "Invalid first expansion start time" );
            if ( nodes[nodes.UpperBound()].StartTime() >= m_endTime )
               throw Error( "Invalid last expansion start time" );
            int N = nodes[0].NumberOfComponents();
            if ( N < 1 )
               throw Error( "Empty or corrupted expansion data" );
            for ( size_type i = 1; i < nodes.Length(); ++i )
            {
               try
               {
                  if ( !nodes[i].StartTime().IsValid() )
                     throw Error( "Invalid expansion start time" );
                  if ( nodes[i].StartTime() < nodes[i-1].StartTime() )
                     throw Error( "Unordered expansion" );
                  if ( 1 + (nodes[i].StartTime() - nodes[i-1].StartTime()) == 1 )
                     throw Error( "Empty or insignificant time span" );
                  if ( nodes[i].NumberOfComponents() != N )
                     throw Error( "Incoherent expansion dimension" );
                  if ( nodes[i].position < minPos || nodes[i].position >= fileSize )
                     throw Error( "Invalid expansion data position" );
               }
               catch ( const Exception& x )
               {
                  throw Error( x.Message() + ", start time " + nodes[i].StartTime().ToString() + ", order " + String( order ) );
               }
               catch ( ... )
               {
                  throw;
               }
            }
         }
         catch ( const Exception& x )
         {
            throw Error( x.Message() + " for object '" + ix.objectId + "' with origin '" + ix.originId + "'." );
         }
         catch ( ... )
         {
            throw;
         }
      }

   m_constants.Sort();
   m_index.Sort();
}

// ----------------------------------------------------------------------------

void EphemerisFile::Close()
{
   if ( IsOpen() )
   {
      volatile AutoLock lock( m_mutex );
      int n = NumberOfHandles();
      if ( n > 0 )
         throw Error( String().Format( "Invalid call: This EphemerisFile instance has %d active child handle(s).", n ) );
      m_file.Close();
      m_startTime = m_endTime = TimePoint();
      m_metadata = EphemerisMetadata();
      m_constants = EphemerisConstantList();
      m_index = Array<Index>();
   }
}

// ----------------------------------------------------------------------------

EphemerisFile::~EphemerisFile() noexcept( false )
{
   volatile AutoLock lock( m_mutex );
   int n = NumberOfHandles();
   if ( n > 0 )
      std::cerr << IsoString().Format( "** Warning: Destroying an EphemerisFile instance with %d active child handle(s).\n", n );
   m_file.Close();
}

// ----------------------------------------------------------------------------

void EphemerisFile::Serialize( const String& filePath,
                               TimePoint startTime, TimePoint endTime,
                               const SerializableEphemerisObjectDataList& data,
                               const EphemerisMetadata& metadata,
                               const EphemerisConstantList& constants )
{
   // Validate data
   if ( filePath.IsEmpty() )
      throw Error( "Empty file path." );

   if ( endTime < startTime )
      Swap( startTime, endTime );
   if ( 1 + (endTime - startTime) == 1 )
      throw Error( "Empty or insignificant time span." );

   if ( data.IsEmpty() )
      throw Error( "Empty ephemeris data list." );

   for ( const SerializableEphemerisObjectData& o : data )
   {
      if ( o.objectId.IsEmpty() )
         throw Error( "Empty object identifier." );

      try
      {
         if ( o.originId.IsEmpty() )
            throw Error( "Empty origin identifier." );
         if ( o.originId == o.objectId )
            throw Error( "The object and origin identifiers must be different." );

         for ( int order = 0; order < 2; ++order )
         {
            const SerializableEphemerisDataList& data = o.data[order];

            try
            {
               if ( data.IsEmpty() )
               {
                  if ( order > 0 )
                     break;
                  throw Error( "Empty ephemeris data" );
               }

               if ( data[0].startTime != startTime )
                  throw Error( "Invalid first expansion start time" );
               if ( data[data.UpperBound()].startTime >= endTime )
                  throw Error( "Invalid last expansion start time" );
               int N = data[0].expansion.NumberOfComponents();
               if ( N > 4 )
                  throw Error( "Unsupported expansion dimension (" + String( N ) + ')' );
               for ( size_type i = 0; i < data.Length(); ++i )
               {
                  if ( !data[i].startTime.IsValid() )
                     throw Error( "Invalid expansion start time" );
                  try
                  {
                     if ( !data[i].expansion.IsValid() )
                        throw Error( "Invalid expansion data" );
                     if ( i > 0 )
                     {
                        if ( data[i].startTime < data[i-1].startTime )
                           throw Error( "Unordered expansion" );
                        if ( 1 + (data[i].startTime - data[i-1].startTime) == 1 )
                           throw Error( "Empty or insignificant time span" );
                        if ( data[i].expansion.NumberOfComponents() != N )
                           throw Error( "Incoherent expansion dimension" );
                     }
                  }
                  catch ( const Exception& x )
                  {
                     throw Error( x.Message() + ", start time " + data[i].startTime.ToString() );
                  }
                  catch ( ... )
                  {
                     throw;
                  }
               }
            }
            catch ( const Exception& x )
            {
               throw Error( x.Message() + ", order " + String( order ) );
            }
            catch ( ... )
            {
               throw;
            }
         }
      }
      catch ( const Exception& x )
      {
         throw Error( x.Message() + " for object '" + o.objectId + "' with origin '" + o.originId + "'." );
      }
      catch ( ... )
      {
         throw;
      }
   }

   IsoString header;
   IsoStringList indexPositions;
   {
      XMLDocument xml;
      xml.SetXML( "1.0", "UTF-8" );
      xml << new XMLComment( "\nPixInsight Planetary Ephemeris Data Format - XEPH version 1.0"
                             "\nCreated with PixInsight software - http://pixinsight.com/"
                             "\n" );

      XMLElement* root = new XMLElement( "xeph", XMLAttributeList()
         << XMLAttribute( "version", "1.0" )
         << XMLAttribute( "xmlns", "http://www.pixinsight.com/xeph" )
         << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
         << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xeph http://pixinsight.com/xeph/xeph-1.0.xsd" ) );

      xml.SetRootElement( root );

      XMLElement* metadataElement = new XMLElement( *root, "Metadata" );
      *(new XMLElement( *metadataElement, "CreationTime" )) << new XMLText( TimePoint::Now().ToString() );
      *(new XMLElement( *metadataElement, "CreatorOS" )) << new XMLText(
#ifdef __PCL_FREEBSD
                                                   "FreeBSD"
#endif
#ifdef __PCL_LINUX
                                                   "Linux"
#endif
#ifdef __PCL_MACOSX
                                                   "macOS"
#endif
#ifdef __PCL_WINDOWS
                                                   "Windows"
#endif
                                                   );

      *(new XMLElement( *metadataElement, "CreatorApplication" )) << new XMLText( metadata.creatorApplication.IsEmpty() ? pcl::Version::AsString() : metadata.creatorApplication );

      if ( !metadata.title.IsEmpty() )
         *(new XMLElement( *metadataElement, "Title" )) << new XMLText( metadata.title );
      if ( !metadata.briefDescription.IsEmpty() )
         *(new XMLElement( *metadataElement, "BriefDescription" )) << new XMLText( metadata.briefDescription );
      if ( !metadata.description.IsEmpty() )
         *(new XMLElement( *metadataElement, "Description" )) << new XMLText( metadata.description );
      if ( !metadata.organizationName.IsEmpty() )
         *(new XMLElement( *metadataElement, "OrganizationName" )) << new XMLText( metadata.organizationName );
      if ( !metadata.authors.IsEmpty() )
         *(new XMLElement( *metadataElement, "Authors" )) << new XMLText( metadata.authors );
      if ( !metadata.copyright.IsEmpty() )
         *(new XMLElement( *metadataElement, "Copyright" )) << new XMLText( metadata.copyright );

      new XMLElement( *root, "TimeSpan", XMLAttributeList()
         << XMLAttribute( "start", startTime.ToString() )
         << XMLAttribute( "end", endTime.ToString() ) );

      if ( !constants.IsEmpty() )
      {
         StringList items;
         for ( auto c : constants )
            items << String( c.name ).AppendFormat( "=%.15g", c.value );
         *(new XMLElement( *root, "Constants" )) << new XMLText( String().ToSeparated( items, ",\n" ) );
      }

      for ( const SerializableEphemerisObjectData& o : data )
      {
         XMLElement* objectElement = new XMLElement( *root, "Object" );
         objectElement->SetAttribute( "id", o.objectId );
         objectElement->SetAttribute( "origin", o.originId );
         if ( !o.objectName.IsEmpty() )
            objectElement->SetAttribute( "name", o.objectName );

         if ( !o.description.IsEmpty() )
            *(new XMLElement( *objectElement, "Description" )) << new XMLText( o.description );

         for ( int order = 0; order < 2; ++order )
         {
            const SerializableEphemerisDataList& data = o.data[order];
            if ( data.IsEmpty() )
               break;

            XMLElement* indexElement = new XMLElement( *objectElement, "Index" );

            double smallestTimeSpan = endTime - startTime;
            double largestTimeSpan = endTime - data[data.UpperBound()].startTime;
            Vector largestTruncationErrors( 0.0, data[0].expansion.NumberOfComponents() );
            size_type totalCoefficients = 0;
            for ( size_type i = 0; i < data.Length(); ++i )
            {
               if ( i > 0 )
               {
                  double d = data[i].startTime - data[i-1].startTime;
                  if ( d < smallestTimeSpan )
                     smallestTimeSpan = d;
                  if ( d > largestTimeSpan )
                     largestTimeSpan = d;
               }
               for ( int j = 0; j < largestTruncationErrors.Length(); ++j )
               {
                  double e = data[i].expansion.TruncationError( j );
                  if ( e > largestTruncationErrors[j] )
                     largestTruncationErrors[j] = e;
               }
               totalCoefficients += data[i].expansion.NumberOfTruncatedCoefficients();
            }

            IsoString position = IsoString::Random( 16 );
            indexPositions << position;

            indexElement->SetAttribute( "order", String( order ) );
            indexElement->SetAttribute( "position", position );
            indexElement->SetAttribute( "numberOfExpansions", String( data.Length() ) );
            indexElement->SetAttribute( "smallestTimeSpan", String( smallestTimeSpan ) );
            indexElement->SetAttribute( "largestTimeSpan", String( largestTimeSpan ) );
            indexElement->SetAttribute( "dimensions", String( largestTruncationErrors.Length() ) );
            indexElement->SetAttribute( "largestTruncationErrors", String().ToCommaSeparated( largestTruncationErrors ) );
            indexElement->SetAttribute( "totalCoefficients", String( totalCoefficients ) );
         }
      }

      xml.EnableAutoFormatting();
      xml.SetIndentSize( 3 );
      header = xml.Serialize();
   }

   fpos_type position;

   /*
    * Replace index position attributes. This is an iterative algorithm
    * resilient to changes in attribute value lengths.
    */
   for ( size_type n = header.Length(); ; )
   {
      position = sizeof( XEPHFileSignature ) + n;
      int i = 0;
      for ( const SerializableEphemerisObjectData& o : data )
         for ( int order = 0; order < 2; ++order )
         {
            const SerializableEphemerisDataList& data = o.data[order];
            if ( data.IsEmpty() )
               break;
            IsoString indexPosition = IsoString().Format( "%lld", position );
            header.ReplaceString( indexPositions[i].DoubleQuoted(), indexPosition.DoubleQuoted() );
            position += data.Length() * sizeof( IndexNode );
            indexPositions[i++] = indexPosition;
         }
      if ( header.Length() == n )
         break;
      n = header.Length();
   }

   /*
    * Write the XEPH file.
    */
   File file = File::CreateFileForWriting( filePath );

   // 1. XEPH signature.
   file.Write( XEPHFileSignature( uint32( header.Length() ) ) );

   // 2. XEPH header.
   file.Write( reinterpret_cast<const void*>( header.Begin() ), header.Length() );

   // 3. Expansion index.
   Array<IndexNode> index;
   for ( const SerializableEphemerisObjectData& o : data )
      for ( int order = 0; order < 2; ++order )
         for ( const SerializableEphemerisData& d : o.data[order] )
         {
            IndexNode node;
            node.jdi = d.startTime.JDI();
            node.jdf = d.startTime.JDF();
            node.position = position;
            for ( int j = 0; j < d.expansion.NumberOfComponents(); ++j )
               node.n[j] = d.expansion.TruncatedLength( j );
            index << node;
            position += node.NumberOfCoefficients() * sizeof( double );
         }
   file.Write( reinterpret_cast<const void*>( index.Begin() ), index.Size() );

   // 4. Expansion data.
   Array<IndexNode>::const_iterator p = index.ConstBegin();
   for ( const SerializableEphemerisObjectData& o : data )
      for ( int order = 0; order < 2; ++order )
         for ( const SerializableEphemerisData& d : o.data[order] )
         {
            if ( p->position != file.Position() )
               throw Error( String().Format( "EphemerisFile::Serialize(): Internal error: "
                                             "Wrong file position: Now writing at %lld, expected at %lld",
                                             file.Position(), p->position ) );
            ++p;

            for ( int j = 0; j < d.expansion.NumberOfComponents(); ++j )
               file.Write( reinterpret_cast<const void*>( d.expansion.Coefficients()[j].Begin() ),
                           d.expansion.TruncatedLength( j )*sizeof( double ) );
         }

   file.Close();
}

// ----------------------------------------------------------------------------

const EphemerisFile& EphemerisFile::FundamentalEphemerides()
{
   volatile AutoLock lock( s_mutex );

   if ( s_E != nullptr )
      return *s_E;

   String filePath = s_ephFilePath;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/FundamentalEphemeridesFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The fundamental ephemerides file has not been defined." );
   }
   if ( !File::Exists( filePath ) )
      throw Error( "The fundamental ephemerides file does not exist: " + filePath );

   try
   {
      return *(s_E = new EphemerisFile( filePath ));
   }
   catch ( const Exception& x )
   {
      throw Error( "Loading fundamental ephemerides file: " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

const EphemerisFile& EphemerisFile::ShortTermFundamentalEphemerides()
{
   volatile AutoLock lock( s_mutex );

   if ( s_Es != nullptr )
      return *s_Es;

   String filePath = s_ephFilePath_s;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/ShortTermFundamentalEphemeridesFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The short-term fundamental ephemerides file has not been defined." );
   }
   if ( !File::Exists( filePath ) )
      throw Error( "The short-term fundamental ephemerides file does not exist: " + filePath );

   try
   {
      return *(s_Es = new EphemerisFile( filePath ));
   }
   catch ( const Exception& x )
   {
      throw Error( "Loading short-term fundamental ephemerides file: " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

const EphemerisFile& EphemerisFile::AsteroidEphemerides()
{
   volatile AutoLock lock( s_mutex );

   if ( s_A != nullptr )
      return *s_A;

   String filePath = s_astFilePath;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/AsteroidEphemeridesFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The asteroid ephemerides file has not been defined." );
   }
   if ( !File::Exists( filePath ) )
      throw Error( "The asteroid ephemerides file does not exist: " + filePath );

   try
   {
      return *(s_A = new EphemerisFile( filePath ));
   }
   catch ( const Exception& x )
   {
      throw Error( "Loading asteroid ephemerides file: " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

const EphemerisFile& EphemerisFile::ShortTermAsteroidEphemerides()
{
   volatile AutoLock lock( s_mutex );

   if ( s_As != nullptr )
      return *s_As;

   String filePath = s_astFilePath_s;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/ShortTermAsteroidEphemeridesFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The short-term asteroid ephemerides file has not been defined." );
   }
   if ( !File::Exists( filePath ) )
      throw Error( "The short-term asteroid ephemerides file does not exist: " + filePath );

   try
   {
      return *(s_As = new EphemerisFile( filePath ));
   }
   catch ( const Exception& x )
   {
      throw Error( "Loading short-term asteroid ephemerides file: " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

const EphemerisFile& EphemerisFile::NutationModel()
{
   volatile AutoLock lock( s_mutex );

   if ( s_N != nullptr )
      return *s_N;

   String filePath = s_nutFilePath;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/NutationModelFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The nutation model file has not been defined." );
   }
   if ( !File::Exists( filePath ) )
      throw Error( "The nutation model file does not exist: " + filePath );

   try
   {
      return *(s_N = new EphemerisFile( filePath ));
   }
   catch ( const Exception& x )
   {
      throw Error( "Loading nutation model file: " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

const EphemerisFile& EphemerisFile::ShortTermNutationModel()
{
   volatile AutoLock lock( s_mutex );

   if ( s_Ns != nullptr )
      return *s_Ns;

   String filePath = s_nutFilePath_s;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/ShortTermNutationModelFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The short-term nutation model file has not been defined." );
   }
   if ( !File::Exists( filePath ) )
      throw Error( "The short-term nutation model file does not exist: " + filePath );

   try
   {
      return *(s_Ns = new EphemerisFile( filePath ));
   }
   catch ( const Exception& x )
   {
      throw Error( "Loading short-term nutation model file: " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

String EphemerisFile::DeltaTDataFilePath()
{
   volatile AutoLock lock( s_mutex );

   String filePath = s_deltaTFilePath;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/DeltaTDataFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The DeltaT database file has not been defined." );
   }

   /*
    * Just check for file existence at this point - the database will be loaded
    * and parsed (irreversibly) upon the first call (explicit or implicit) to
    * TimePoint::DeltaT().
    */
   if ( !File::Exists( filePath ) )
      throw Error( "The DeltaT database file does not exist: " + filePath );
   return filePath;
}

// ----------------------------------------------------------------------------

String EphemerisFile::DeltaATDataFilePath()
{
   volatile AutoLock lock( s_mutex );

   String filePath = s_deltaATFilePath;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/DeltaATDataFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The DeltaAT database file has not been defined." );
   }

   /*
    * Just check for file existence at this point - the database will be loaded
    * and parsed (irreversibly) upon the first call (explicit or implicit) to
    * TimePoint::DeltaAT().
    */
   if ( !File::Exists( filePath ) )
      throw Error( "The DeltaAT database file does not exist: " + filePath );
   return filePath;
}

// ----------------------------------------------------------------------------

String EphemerisFile::CIP_ITRSDataFilePath()
{
   volatile AutoLock lock( s_mutex );

   String filePath = s_cipITRSFilePath;
   if ( filePath.IsEmpty() )
   {
      filePath = PixInsightSettings::GlobalString( "Application/CIP_ITRSDataFilePath" );
      if ( filePath.IsEmpty() )
         throw Error( "The CIP_ITRS database file has not been defined." );
   }

   /*
    * Just check for file existence at this point - the database will be loaded
    * and parsed (irreversibly) upon the first call (explicit or implicit) to
    * Position::CIP_ITRS().
    */
   if ( !File::Exists( filePath ) )
      throw Error( "The CIP_ITRS database file does not exist: " + filePath );
   return filePath;
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideFundamentalEphemerides( const String& filePath )
{
   volatile AutoLock lock( s_mutex );

   if ( s_E != nullptr )
   {
      delete s_E;
      s_E = nullptr;
   }
   s_ephFilePath = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideShortTermFundamentalEphemerides( const String& filePath )
{
   volatile AutoLock lock( s_mutex );

   if ( s_Es != nullptr )
   {
      delete s_Es;
      s_Es = nullptr;
   }
   s_ephFilePath_s = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideAsteroidEphemerides( const String& filePath )
{
   volatile AutoLock lock( s_mutex );

   if ( s_A != nullptr )
   {
      delete s_A;
      s_A = nullptr;
   }
   s_astFilePath = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideShortTermAsteroidEphemerides( const String& filePath )
{
   volatile AutoLock lock( s_mutex );

   if ( s_As != nullptr )
   {
      delete s_As;
      s_As = nullptr;
   }
   s_astFilePath_s = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideNutationModel( const String& filePath )
{
   volatile AutoLock lock( s_mutex );

   if ( s_N != nullptr )
   {
      delete s_N;
      s_N = nullptr;
   }
   s_nutFilePath = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideShortTermNutationModel( const String& filePath )
{
   volatile AutoLock lock( s_mutex );

   if ( s_Ns != nullptr )
   {
      delete s_Ns;
      s_Ns = nullptr;
   }
   s_nutFilePath_s = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideDeltaTDataFilePath( const String& filePath )
{
   volatile AutoLock lock( s_mutex );
   s_deltaTFilePath = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideDeltaATDataFilePath( const String& filePath )
{
   volatile AutoLock lock( s_mutex );
   s_deltaATFilePath = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisFile::OverrideCIP_ITRSDataFilePath( const String& filePath )
{
   volatile AutoLock lock( s_mutex );
   s_cipITRSFilePath = filePath.Trimmed();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/EphemerisFile.cpp - Released 2018-11-01T11:06:51Z
