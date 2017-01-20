
#include <pcl/Exception.h>
#include <pcl/File.h>
#include <pcl/XML.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <typename Itr>
inline static Itr SkipWhitespace( Itr i, Itr j )
{
   for ( ; i < j; ++i )
      if ( !XML::IsSpaceChar( *i ) )
         break;
   return i;
}

template <typename Itr>
inline static Itr SkipWhitespace( const String& s, Itr i )
{
   return SkipWhitespace( i, s.End() );
}

template <typename Itr>
inline static Itr SkipTrailingWhitespace( Itr i, Itr j )
{
   for ( Itr k = j; i < j; --j )
      if ( !XML::IsSpaceChar( *--k ) )
         break;
   return j;
}

template <typename Itr>
inline static Itr FindNextChar( Itr i, Itr j, char16_type c )
{
   for ( ; i < j; ++i )
      if ( *i == c )
         break;
   return i;
}

template <typename Itr>
inline static Itr FindNextChar( const String& s, Itr i, char16_type c )
{
   for ( ; i < s.End(); ++i )
      if ( *i == c )
         break;
   return i;
}

template <typename Itr>
inline static Itr FindNextSpace( const String& s, Itr i, Itr j )
{
   for ( ; i < j; ++i )
      if ( XML::IsSpaceChar( *i ) )
         break;
   return i;
}

inline static String::const_iterator FindFirstSpace( const String& s )
{
   String::const_iterator i = s.Begin();
   for ( ; i < s.End(); ++i )
      if ( XML::IsSpaceChar( *i ) )
         break;
   return i;
}

template <typename Itr>
inline static Itr FindClosingChar( const String& s, Itr i, char16_type l, char16_type r )
{
   for ( size_type n = 1; i < s.End(); ++i )
      if ( *i == r )
      {
         if ( --n == 0 )
            break;
      }
      else if ( *i == l )
         ++n;
   return i;
}

template <typename Itr>
inline static bool IsToken( const String& s, Itr i, const char* t )
{
   int n = 0;
   while ( t[n] != '\0' ) ++n;
   if ( n > 0 )
      if ( s.End() - i > n )
         while ( *i++ == *t++ )
            if ( --n == 0 )
               return true;
   return false;
}

template <typename Itr>
inline static Itr FindToken( const String& s, Itr i, const char* t )
{
   size_type p = s.Find( t, s.IndexAt( i ) );
   return (p != String::notFound) ? s.At( p ) : s.End();
}

template <typename Itr>
inline static Itr FindToken( const String& s, Itr i, const String& t )
{
   size_type p = s.Find( t, s.IndexAt( i ) );
   return (p != String::notFound) ? s.At( p ) : s.End();
}

// ----------------------------------------------------------------------------

String XML::TrimmedSpaces( String::const_iterator i, String::const_iterator j )
{
   i = SkipWhitespace( i, j );
   return String( i, SkipTrailingWhitespace( i, j ) );
}

String XML::TrimmedSpaces( const String& text )
{
   String::const_iterator i = SkipWhitespace( text, text.Begin() );
   String::const_iterator j = SkipTrailingWhitespace( i, text.End() );
   if ( i == text.Begin() )
      if ( j == text.End() )
         return text;
   return String( i, j );
}

String XML::CollapsedSpaces( String::const_iterator i, String::const_iterator j )
{
   String transform;
   for ( String::const_iterator i0 = i; ; )
   {
      if ( i == j )
      {
         transform.Append( i0, i );
         break;
      }

      if ( IsSpaceChar( *i ) )
      {
         transform.Append( i0, i );
         transform.Append( char16_type( 0x20 ) );
         i0 = i = SkipWhitespace( ++i, j );
      }
      else
         ++i;
   }
   return transform;
}

String XML::CollapsedSpaces( const String& text )
{
   String transform;
   for ( String::const_iterator i0 = text.Begin(), i = i0; ; )
   {
      if ( i == text.End() )
      {
         if ( i0 == text.Begin() )
            transform = text;
         else
            transform.Append( i0, i );
         break;
      }

      if ( IsSpaceChar( *i ) )
      {
         transform.Append( i0, i );
         transform.Append( char16_type( 0x20 ) );
         i0 = i = SkipWhitespace( text, ++i );
      }
      else
         ++i;
   }
   return transform;
}

String XML::DecodedText( String::const_iterator i, String::const_iterator j )
{
   String text;
   if ( i < j )
      for ( String::const_iterator i0 = i; ; )
      {
         if ( *i == '&' )
         {
            String::const_iterator e = i;
            String::const_iterator f = FindNextChar( ++e, j, ';' );
            if ( f != j )
            {
               text.Append( i0, i );
               text.Append( ReferenceValue( e, f ) );
               i = f;
               if ( ++i == j )
                  break;
               i0 = i;
               continue;
            }
         }

         if ( ++i == j )
         {
            text.Append( i0, i );
            break;
         }
      }

   return text;
}

String XML::DecodedText( const String& s )
{
   String text;
   if ( !s.IsEmpty() )
      for ( String::const_iterator i0 = s.Begin(), i = i0; ; )
      {
         if ( *i == '&' )
         {
            String::const_iterator e = i;
            String::const_iterator f = FindNextChar( ++e, s.End(), ';' );
            if ( f != s.End() )
            {
               text.Append( i0, i );
               text.Append( ReferenceValue( e, f ) );
               i = f;
               if ( ++i == s.End() )
                  break;
               i0 = i;
               continue;
            }
         }

         if ( ++i == s.End() )
         {
            if ( i0 == s.Begin() )
               text = s;
            else
               text.Append( i0, i );
            break;
         }
      }

   return text;
}

String XML::EncodedText( const String& text, bool apos )
{
   String s = text;
   s.ReplaceString( "&", "&amp;" );
   s.ReplaceString( "\"", "&quot;" );
   s.ReplaceString( "<", "&lt;" );
   s.ReplaceString( ">", "&gt;" );
   if ( apos )
      s.ReplaceString( "\'", "&apos;" );
   return s;
}

// ----------------------------------------------------------------------------

void XMLAttributeList::Parse( const String& text )
{
   Clear();
   try
   {
      for ( String::const_iterator i = text.Begin(); ; )
      {
         i = SkipWhitespace( text, i );
         if ( i == text.End() )
            break;

         if ( !XML::IsNameStartChar( *i ) )
            throw Error( String().Format( "Invalid attribute name starting character #x%x", int( *i ) ) );

         String::const_iterator j = i;
         while ( ++j < text.End() )
         {
            if ( *j == '=' || XML::IsSpaceChar( *j ) )
               break;
            if ( !XML::IsNameChar( *j ) )
               throw Error( String().Format( "Invalid attribute name character #x%x", int( *j ) ) );
         }

         String::const_iterator k = SkipWhitespace( text, j );
         if ( k == text.End() || *k != '=' )
            throw Error( "Expected equal sign." );

         k = SkipWhitespace( text, ++k );
         if ( k == text.End() )
            throw Error( "Missing attribute value." );
         char16_type q = *k;
         if ( q != '\"' && q != '\'' )
            throw Error( "Expected starting double or single quote." );

         k = SkipWhitespace( text, ++k );
         String::const_iterator l = FindNextChar( text, k, q );
         if ( l == text.End() )
            throw Error( "Unmatched double or single quote." );

         SetAttribute( String( i, j ), XML::CollapsedSpaces( XML::DecodedText( k, SkipTrailingWhitespace( k, l ) ) ) );

         i = l+1;
      }
   }
   catch ( const Exception& x )
   {
      throw Error( "Parsing XML attribute list: " + x.Message() );
   }
}

void XMLAttributeList::Serialize( IsoString& text ) const
{
   for ( const_iterator i = m_list.Begin(); ; )
   {
      text << i->Name().ToUTF8() << "=\"" << i->EncodedValue().ToUTF8() << '\"';
      if ( ++i == m_list.End() )
         break;
      text << ' ';
   }
}

// ----------------------------------------------------------------------------

bool XMLNode::NLAfter( const XMLNode& node ) const
{
   return node.NodeType() != XMLNodeType::Text || !dynamic_cast<const XMLText&>( node ).IsPreserveSpaces();
}

// ----------------------------------------------------------------------------

void XMLElement::Serialize( IsoString& text, int indentSize, int level ) const
{
   if ( indentSize > 0 )
      if ( level > 0 )
         text.Append( ' ', indentSize*level );
   text << '<' << m_name.ToUTF8();

   if ( HasAttributes() )
   {
      text << ' ';
      SerializeAttributes( text );
   }

   if ( IsEmpty() )
      text << "/>";
   else
   {
      text << ">" /*<< IsoString().Format( "(%d)", ChildCount() )*/;

      if ( indentSize >= 0 && First().NLAfter( *this ) )
      {
         text << '\n';
         First().Serialize( text, indentSize, level+1 );
      }
      else
         First().Serialize( text, indentSize, 0 );

      for ( const_iterator j = Begin(), i = j; ++j != End(); ++i )
         if ( indentSize >= 0 && j->NLAfter( *i ) )
         {
            text << '\n';
            j->Serialize( text, indentSize, level+1 );
         }
         else
            j->Serialize( text, indentSize, 0 );

      if ( indentSize >= 0 && NLAfter( Last() ) )
      {
         text << '\n';
         text.Append( ' ', indentSize*level );
      }
      text << "</" << m_name.ToUTF8() << '>';
   }
}

// ----------------------------------------------------------------------------

void XMLText::Serialize( IsoString& text, int indentSize, int level ) const
{
   if ( indentSize > 0 )
      if ( !m_preserveSpaces )
         text.Append( ' ', indentSize*level );
   text << XML::EncodedText( m_text ).ToUTF8();
}

// ----------------------------------------------------------------------------

void XMLCDATA::Serialize( IsoString& text, int indentSize, int level ) const
{
   if ( indentSize > 0 )
      text.Append( ' ', indentSize*level );
   IsoString cdata = m_cdata.ToUTF8();
   cdata.DeleteString( "]]>" ); // forbidden sequence
   text << "<![CDATA[" << cdata << "]]>";
}

// ----------------------------------------------------------------------------

void XMLProcessingInstructions::Serialize( IsoString& text, int indentSize, int level ) const
{
   if ( indentSize > 0 )
      text.Append( ' ', indentSize*level );
   IsoString instructions = m_instructions.ToUTF8();
   instructions.DeleteString( "?>" ); // forbidden sequence
   text << "<?" << m_target.ToUTF8() << ' ' << instructions << "?>";
}

// ----------------------------------------------------------------------------

void XMLComment::Serialize( IsoString& text, int indentSize, int level ) const
{
   if ( indentSize > 0 )
      text.Append( ' ', indentSize*level );
   IsoString comment = m_comment.ToUTF8();
   comment.DeleteString( "--" ); // forbidden sequence
   text << "<!--" << comment << (comment.EndsWith( '-' ) ? "->" : "-->");
}

// ----------------------------------------------------------------------------

void XMLUnknownElement::Serialize( IsoString& text, int indentSize, int level ) const
{
   if ( indentSize > 0 )
      text.Append( ' ', indentSize*level );
   text << "<!" << m_name.ToUTF8() << ' ' << m_parameters.ToUTF8() << '>';
}

// ----------------------------------------------------------------------------

void XMLDeclaration::Serialize( IsoString& text ) const
{
   text << "<?xml version=\"" << (m_version.IsEmpty() ? IsoString( "1.0" ) : IsoString( m_version ))
        << "\" encoding=\"" << (m_encoding.IsEmpty() ? IsoString( "UTF-8" ) : IsoString( m_encoding )) << '\"';
   if ( m_standalone )
      text << " standalone=\"yes\"";
   text << "?>";
}

// ----------------------------------------------------------------------------

void XMLDocTypeDeclaration::Serialize( IsoString& text ) const
{
   if ( !m_name.IsEmpty() )
   {
      text << "<!DOCTYPE " << m_name.ToUTF8();
      if ( !m_definition.IsEmpty() )
         text << ' ' << m_definition.ToUTF8();
      text << '>';
   }
}

// ----------------------------------------------------------------------------

IsoString XMLDocument::Serialize( int indentSize ) const
{
   indentSize = Range( indentSize, -1, 8 );

   IsoString text;

   if ( m_xml.IsDefined() )
   {
      m_xml.Serialize( text );
      if ( indentSize >= 0 )
         text << '\n';
   }

   if ( m_docType.IsDefined() )
   {
      m_docType.Serialize( text );
      if ( indentSize >= 0 )
         text << '\n';
   }

   for ( const XMLNode& node : m_nodes )
   {
      node.Serialize( text, indentSize, 0 );
      if ( indentSize >= 0 )
         text << '\n';
   }

   return text;
}

void XMLDocument::SerializeToFile( const String& path, int indentSize ) const
{
   File::WriteTextFile( path, Serialize( indentSize ) );
}

// ----------------------------------------------------------------------------

void XMLDocument::AddNode( XMLNode* node )
{
   try
   {
      if ( node == nullptr )
         throw Error( "Null pointer specified." );
      if ( node->IsChildNode() )
         throw Error( "Child node specified." );

      m_nodes.Append( node );
      node->m_type.SetFlag( XMLNodeType::ChildNode );
   }
   catch ( const Exception& x )
   {
      throw Error( "XMLDocument::AddNode(): " + x.Message() );
   }
}

void XMLDocument::SetRootElement( XMLElement* element )
{
   try
   {
      if ( element == nullptr )
         throw Error( "Null pointer specified." );
      if ( element->IsChildNode() )
         throw Error( "Child node specified." );
      if ( !element->IsRootElement() )
         throw Error( "Internal error: Inconsistent node hierarchy." );
      if ( m_root != nullptr )
         throw Error( "Already have a document root element." );

      m_nodes.Append( element );
      m_root = element;
      m_root->m_type.SetFlag( XMLNodeType::ChildNode );
   }
   catch ( const Exception& x )
   {
      throw Error( "XMLDocument::SetRootElement(): " + x.Message() );
   }
}

void XMLDocument::Clear()
{
   m_xml = XMLDeclaration();
   m_docType = XMLDocTypeDeclaration();
   m_nodes.Destroy();
   m_root = nullptr;
   m_location = XMLNodeLocation();
}

// ----------------------------------------------------------------------------

struct XMLTag
{
   String                 name;        // the tag name
   String                 parameters;  // tag contents after the name, trimmed
   bool                   start : 1;   // true if this is a start-tag
   bool                   end   : 1;   // true if this is a start-tag or empty element
   bool                   PI    : 1;   // true if this is a processing instructions tag
   String::const_iterator next;        // after the end-tag delimiter

   XMLTag( const String& s, String::const_iterator i ) :
      name(), parameters(), start( false ), end( false ), PI( false ), next( i )
   {
      try
      {
         i = SkipWhitespace( s, i );
         if ( i == s.End() )
            throw Error( "Missing tag name." );
         if ( *i == '/' )
         {
            end = true;
            ++i;
         }
         else
         {
            start = true;
            if ( *i == '?' )
            {
               PI = end = true;
               ++i;
            }
         }

         if ( !XML::IsNameStartChar( *i ) )
            throw Error( String().Format( "Invalid tag name starting character #x%x", int( *i ) ) );

         String::const_iterator j = FindClosingChar( s, i, '<', '>' );
         if ( j == s.End() )
            throw Error( "Unmatched start-tag delimiter." );
         if ( j == i )
            throw Error( "Missing tag name." );
         next = j + 1;
         if ( PI )
         {
            if ( *--j != '?' )
               throw Error( "Invalid PI tag syntax: Expected '?>' tag delimiter." );
            if ( j == i )
               throw Error( "Missing PI tag name." );
         }
         else if ( *(j-1) == '/' )
         {
            if ( end )
               throw Error( "Invalid end-tag syntax: Unexpected '/>' tag delimiter." );
            end = true;
            if ( --j == i )
               throw Error( "Missing tag name." );
         }

         String::const_iterator k = FindNextSpace( s, i, j );
         for ( String::const_iterator j = i; ++j < k; )
            if ( !XML::IsNameChar( *j ) )
               throw Error( String().Format( "Invalid tag name character #x%x", int( *j ) ) );
         name = String( i, k );

         k = SkipWhitespace( s, k );
         if ( k < j )
            parameters = String( k, SkipTrailingWhitespace( k, j ) );
      }
      catch ( const Exception& x )
      {
         throw Error( "Parsing XML tag: " + x.Message() );
      }
   }
};

void XMLDocument::Parse( const String& text )
{
   Clear();

   try
   {
      XMLElement* currentElement = nullptr;
      String skipName;
      size_type skipCount = 0;

      for ( String::const_iterator i = text.Begin(), i0 = i; ; )
      {
         for ( ; i0 < i; ++i0 )
            if ( XML::IsLineBreakChar( *i0 ) )
               ++m_location.line, m_location.column = 0;
            else
               ++m_location.column;

         String::const_iterator j = i;
         bool allspaces = true;
         for ( ; j < text.End(); ++j )
         {
            if ( *j == '<' )
               break;

            if ( XML::IsLineBreakChar( *j ) )
               ++m_location.line, m_location.column = 0;
            else
            {
               if ( allspaces )
                  if ( !XML::IsWhiteSpaceChar( *j ) )
                  {
                     if ( currentElement == nullptr )
                        if ( skipCount == 0 )
                           if ( !m_parserOptions.IsFlagSet( XMLParserOption::IgnoreStrayCharacters ) )
                              throw Error( String().Format( "Stray character #x%x outside markup.", int( *j ) ) );
                     allspaces = false;
                  }
               ++m_location.column;
            }
         }

         if ( j > i )
            if ( skipCount == 0 )
               if ( currentElement != nullptr )
                  if ( !allspaces || currentElement->HasText() )
                     currentElement->AddChildNode( new XMLText( XML::DecodedText( i, j ),
                                                         !m_parserOptions.IsFlagSet( XMLParserOption::NormalizeTextSpaces ) ), m_location );
         if ( j == text.End() )
            break;

         i0 = j;

         j = SkipWhitespace( text, j+1 );
         if ( j == text.End() )
            throw Error( "Unmatched start-tag delimiter." );

         if ( *j == '/' || XML::IsNameStartChar( *j ) )
         {
            XMLTag t( text, j );

            if ( t.start )
            {
               if ( skipCount == 0 )
               {
                  if ( m_filter == nullptr ||
                       (*m_filter)( currentElement, t.name ) &&
                       (*m_filter)( currentElement, t.name, XMLAttributeList( t.parameters ) ) )
                  {
                     XMLElement* element = new XMLElement( t.name, XMLAttributeList( t.parameters ) );

                     if ( currentElement != nullptr )
                        currentElement->AddChildNode( element, m_location );
                     else
                     {
                        m_nodes << element;
                        if ( m_root == nullptr )
                           m_root = element;
                     }

                     if ( !t.end )
                        currentElement = element;
                  }
                  else
                  {
                     if ( !t.end )
                     {
                        skipName = t.name;
                        skipCount = 1;
                     }
                  }
               }
               else
               {
                  if ( !t.end )
                     if ( t.name == skipName )
                        ++skipCount;
               }
            }
            else
            {
               if ( skipCount == 0 )
               {
                  if ( currentElement == nullptr )
                     throw Error( "Stray end-tag '" + t.name + "'"  );
                  if ( t.name != currentElement->Name() )
                     throw Error( "Unexpected end-tag '/" + t.name + "'; expected '/" + currentElement->Name() + "'" );

                  currentElement = currentElement->ParentElement();
               }
               else
               {
                  if ( t.name == skipName )
                     --skipCount;
               }
            }

            i = t.next;
         }
         else if ( *j == '!' )
         {
            ++j;
            if ( IsToken( text, j, "--" ) )
            {
               j += 2;
               String::const_iterator k = FindToken( text, j, "-->" );
               if ( k == text.End() )
                  throw Error( "Unmatched comment start." );

               if ( !m_parserOptions.IsFlagSet( XMLParserOption::IgnoreComments ) )
                  if ( skipCount == 0 )
                  {
                     XMLComment* comment = new XMLComment( String( j, k ) );
                     if ( currentElement != nullptr )
                        currentElement->AddChildNode( comment, m_location );
                     else
                        m_nodes << comment;
                  }

               i = k + 3;
            }
            else if ( IsToken( text, j, "DOCTYPE" ) )
            {
               if ( m_root != nullptr || skipCount > 0 )
                  throw Error( "Invalid DOCTYPE declaration after the root element." );
               if ( m_docType.IsDefined() )
                  throw Error( "Duplicate DOCTYPE declaration." );

               XMLTag t( text, j );
               if ( t.end || t.parameters.IsEmpty() )
                  throw Error( "Invalid DOCTYPE tag syntax." );
               String name, definition;
               String::const_iterator k = FindFirstSpace( t.parameters );
               if ( k != t.parameters.End() )
               {
                  name = String( t.parameters.Begin(), k );
                  definition = String( SkipWhitespace( t.parameters, k+1 ), t.parameters.End() );
               }
               else
                  name = t.parameters;

               m_docType = XMLDocTypeDeclaration( name, definition );

               i = t.next;
            }
            else if ( IsToken( text, j, "[CDATA[" ) )
            {
               j += 7;
               String::const_iterator k = FindToken( text, j, "]]>" );
               if ( k == text.End() )
                  throw Error( "Unmatched CDATA start-tag." );

               if ( skipCount == 0 )
               {
                  if ( currentElement == nullptr )
                     throw Error( "Invalid CDATA section outside an element." );
                  currentElement->AddChildNode( new XMLCDATA( String( j, k ) ), m_location );
               }

               i = k + 3;
            }
            else
            {
               XMLTag t( text, j );

               if ( skipCount == 0 )
                  if ( !m_parserOptions.IsFlagSet( XMLParserOption::IgnoreUnknownElements ) )
                  {
                     XMLUnknownElement* element = new XMLUnknownElement( t.name, t.parameters );
                     if ( currentElement != nullptr )
                        currentElement->AddChildNode( element, m_location );
                     else
                        m_nodes << element;
                  }

               i = t.next;
            }
         }
         else if ( *j == '?' )
         {
            XMLTag t( text, j );
            if ( !t.PI )
               throw Error( "Parsing processing instructions tag: Internal error." );

            if ( t.name == "xml" )
            {
               if ( m_root != nullptr || skipCount > 0 )
                  throw Error( "Invalid XML declaration after the root element." );
               if ( m_xml.IsDefined() )
                  throw Error( "Duplicate XML declaration." );

               XMLAttributeList a( t.parameters );

               String version = a.AttributeValue( "version" );
               if ( version.IsEmpty() )
                  throw Error( "Missing XML version attribute." );

               String encoding = a.AttributeValue( "encoding" );
               if ( !encoding.IsEmpty() )
                  if (    encoding.CompareIC( "UTF-8" ) && encoding.CompareIC( "UTF8" )
                     && encoding.CompareIC( "UTF-16" ) && encoding.CompareIC( "UTF16" )
                     && encoding.CompareIC( "ISO-8859-1" ) )
                     throw Error( "Unsupported or invalid document encoding '" + encoding + "'" );

               String standalone = a.AttributeValue( "standalone" );
               if ( !standalone.IsEmpty() )
                  if ( standalone.CompareIC( "yes" ) && standalone.CompareIC( "no" ) )
                     throw Error( "Invalid document standalone attribute value '" + standalone + "'" );

               m_xml = XMLDeclaration( version, encoding, standalone.CompareIC( "yes" ) == 0 );
            }
            else
            {
               if ( skipCount == 0 )
               {
                  XMLProcessingInstructions* pi = new XMLProcessingInstructions( t.name, t.parameters );
                  if ( currentElement != nullptr )
                     currentElement->AddChildNode( pi, m_location );
                  else
                     m_nodes << pi;
               }
            }

            i = t.next;
         }
         else
         {
            throw Error( String().Format( "Invalid name start character #x%x", int( *j ) ) );
         }
      }

      if ( currentElement != nullptr )
         throw Error( "Incomplete element definition: Expected end-tag '/" + currentElement->Name()  + "'" );

      if ( skipCount > 0 )
         throw Error( "Incomplete element definition: Expected end-tag '/" + skipName  + "'" );

      if ( m_root == nullptr )
         throw Error( "No root element has been defined." );
   }
   catch ( const Exception& x )
   {
      throw Error( String().Format( "Parsing XML document (line=%d col=%d): ", m_location.line+1, m_location.column+1 ) + x.Message() );
   }
}

// ----------------------------------------------------------------------------

} // pcl
