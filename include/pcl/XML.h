
#ifndef __PCL_XML_h
#define __PCL_XML_h

/// \file pcl/XML.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_ReferenceArray_h
#include <pcl/ReferenceArray.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS XMLDocument;
class PCL_CLASS XMLElement;

/*!
 * \class XML
 * \brief Utility functions and data for XML document parsing and generation.
 */
class PCL_CLASS XML
{
public:

   /*
    * Not an instantiable class.
    */
   XML() = delete;
   XML( const XML& ) = delete;

   /*!
    * Returns true iff the specified character \a c is either a white space
    * (#x20) or a tabulator (#9) character.
    */
   template <typename T>
   static bool IsWhiteSpaceChar( T c )
   {
      return c == 0x20 || c == 9;
   }

   /*!
    * Returns true iff the specified character \a c is either a line feed
    * (#x0A) or a carriage return (#0D) control character.
    */
   template <typename T>
   static bool IsLineBreakChar( T c )
   {
      return c == 0x0A || c == 0x0D;
   }

   /*!
    * Returns true iff the specified character \a c is an XML space character:
    *
    * https://www.w3.org/TR/xml11/#NT-S
    */
   template <typename T>
   static bool IsSpaceChar( T c )
   {
      return IsWhiteSpaceChar( c ) || IsLineBreakChar( c );
   }

   /*!
    * Returns true iff the specified character \a c is an XML NameStartChar:
    *
    * https://www.w3.org/TR/xml11/#NT-NameStartChar
    */
   template <typename T>
   static bool IsNameStartChar( T c )
   {
      return c >= T( 'a' ) && c <= T( 'z' )
         ||  c >= T( 'A' ) && c <= T( 'Z' )
         ||  c == T( '_' )
         ||  c == T( ':' )
         ||  c >= 0xC0    && c <= 0xD6
         ||  c >= 0xD8    && c <= 0xF6
         ||  c >= 0xF8    && c <= 0x2FF
         ||  c >= 0x370   && c <= 0x37D
         ||  c >= 0x37F   && c <= 0x1FFF
         ||  c >= 0x200C  && c <= 0x200D
         ||  c >= 0x2070  && c <= 0x218F
         ||  c >= 0x2C00  && c <= 0x2FEF
         ||  c >= 0x3001  && c <= 0xD7FF
         ||  c >= 0xF900  && c <= 0xFDCF
         ||  c >= 0xFDF0  && c <= 0xFFFD
         ||  uint32( c ) >= 0x10000 && uint32( c ) <= 0xEFFFF;
   }

   /*!
    * Returns true iff the specified character \a c is an XML NameChar:
    *
    * https://www.w3.org/TR/xml11/#NT-NameChar
    */
   template <typename T>
   static bool IsNameChar( T c )
   {
      return IsNameStartChar( c )
         ||  c >= T( '0' ) && c <= T( '9' )
         ||  c == T( '-' )
         ||  c == T( '.' )
         ||  c == 0xB7
         ||  c >= 0x0300  && c <= 0x036F
         ||  c >= 0x203F  && c <= 0x2040;
   }

   /*!
    * Returns true iff the specified character \a c is an XML RestrictedChar:
    *
    * https://www.w3.org/TR/xml11/#NT-RestrictedChar
    */
   template <typename T>
   static bool IsRestrictedChar( T c )
   {
      return c >= 0x00 && c <= 0x08
         ||  c >= 0x0B && c <= 0x0C
         ||  c >= 0x0E && c <= 0x1F
         ||  c >= 0x7F && c <= 0x84
         ||  c >= 0x86 && c <= 0x9F;
   }

   /*!
    * Returns true iff the specified \a name is a valid XML qualified element
    * or attribute name:
    *
    * https://www.w3.org/TR/xml-names/#ns-qualnames
    */
   static bool IsValidName( const String& name )
   {
      if ( !name.IsEmpty() )
         if ( IsNameStartChar( *name ) )
            for ( String::const_iterator i = name.Begin(); ; )
            {
               if ( ++i == name.End() )
                  return true;
               if ( !IsNameChar( *i ) )
                  break;
            }
      return false;
   }

   /*!
    * Returns a copy of the text fragment defined by the range [i,j) of string
    * iterators with all leading and trailing space characters removed.
    */
   static String TrimmedSpaces( String::const_iterator i, String::const_iterator j );

   /*!
    * Returns a copy of the specified \a text with all leading and trailing
    * space characters removed.
    */
   static String TrimmedSpaces( const String& text );

   /*!
    * Returns a copy of the text fragment defined by the range [i,j) of string
    * iterators with all sequences of one or more space characters replaced
    * with single white space characters (#x20).
    */
   static String CollapsedSpaces( String::const_iterator i, String::const_iterator j );

   /*!
    * Returns a copy of the specified \a text with all sequences of one or more
    * space characters replaced with single white space characters (#x20).
    */
   static String CollapsedSpaces( const String& text );

   /*!
    * Returns a copy of the text fragment defined by the range [i,j) of string
    * iterators with all XML references replaced by their corresponding UTF-16
    * characters.
    *
    * Both entity and character references are decoded by this function. For
    * entity references, the entire set of XML reference names is supported:
    *
    * http://www.w3.org/TR/xml-entity-names/
    *
    * Character references are interpreted as defined in the XML specification:
    *
    * https://www.w3.org/TR/xml11/#NT-CharRef
    */
   static String DecodedText( String::const_iterator i, String::const_iterator j );

   /*!
    * Returns a copy of the specified \a text with all XML references replaced
    * by their corresponding UTF-16 characters.
    *
    * See DecodedText( String::const_iterator, String::const_iterator ) for a
    * detailed description.
    */
   static String DecodedText( const String& text );

   /*!
    * Returns a copy of the text fragment defined by the range [i,j) of string
    * iterators with all occurences of '&', '<', '>' and '"' replaced with
    * their corresponding entity references. If \a apos is true, single quotes
    * will also be replaced with "apos" entities.
    */
   static String EncodedText( String::const_iterator i, String::const_iterator j, bool apos = true )
   {
      return EncodedText( String( i, j ), apos );
   }

   /*!
    * Returns a copy of the specified \a text with all occurences of '&', '<',
    * '>', '"' and "'" replaced with their corresponding entity references. If
    * \a apos is true, single quotes will also be replaced with "apos"
    * entities.
    */
   static String EncodedText( const String& text, bool apos = true );

   /*!
    * Returns the Unicode value (encoded as UTF-16) corresponding to an XML
    * reference defined by the range [i,j) of string iterators:
    *
    * https://www.w3.org/TR/xml11/#NT-Reference
    *
    * Both entity and character references are decoded by this function. For
    * entity references, the entire set of XML reference names is supported:
    *
    * http://www.w3.org/TR/xml-entity-names/
    *
    * Character references are interpreted as defined in the XML specification:
    *
    * https://www.w3.org/TR/xml11/#NT-CharRef
    */
   static String ReferenceValue( String::const_iterator i, String::const_iterator j );

   /*!
    * Returns the Unicode value (encoded as UTF-16) corresponding to the
    * specified XML \a reference.
    *
    * See ReferenceValue( String::const_iterator, String::const_iterator ) for
    * a detailed description.
    */
   static String ReferenceValue( const String& reference )
   {
      return ReferenceValue( reference.Begin(), reference.End() );
   }
};

/*!
 * \class XMLComponent
 * \brief Root class of all XML document component classes.
 *
 * %XMLComponent supports the hierarchical structure of an XML document by
 * implementing the basic concept of <em>parent element</em>.
 *
 * \sa XMLNode
 */
class PCL_CLASS XMLComponent
{
public:

   /*!
    * Default constructor. Constructs a default %XMLComment object with no
    * parent element.
    */
   XMLComponent() = default;

   /*!
    * Copy constructor.
    */
   XMLComponent( const XMLComponent& ) = default;

   /*!
    * Returns a pointer to the parent XML element of this component, or
    * \c nullptr if this object has no parent element.
    */
   XMLElement* ParentElement() const
   {
      return m_parent;
   }

private:

   XMLElement* m_parent = nullptr;

   friend class XMLElement;
};

/*!
 * \namespace XMLNodeType
 * \brief     XML node types.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>XMLNodeType::Unknown</td>                <td>Represents an unsupported XML node type.</td></tr>
 * <tr><td>XMLNodeType::Element</td>                <td>An XML element.</td></tr>
 * <tr><td>XMLNodeType::Text</td>                   <td>A text block inside an element's contents.</td></tr>
 * <tr><td>XMLNodeType::CDATA</td>                  <td>A CDATA section.</td></tr>
 * <tr><td>XMLNodeType::ProcessingInstructions</td> <td>A processing instructions (PI) section.</td></tr>
 * <tr><td>XMLNodeType::PI</td>                     <td>A synonym to ProcessingInstructions.</td></tr>
 * <tr><td>XMLNodeType::Comment</td>                <td>A comment block.</td></tr>
 * </table>
 */
namespace XMLNodeType
{
   enum mask_type
   {
      Undefined =              0x00000000,
      ChildNode =              0x80000000,
      Unknown =                0x10000000,
      Element =                0x00000001,
      Text =                   0x00000002,
      CDATA =                  0x00000004,
      ProcessingInstructions = 0x00000008,
      Comment =                0x00000010
   };
}

/*!
 * \struct XMLNodeLocation
 * \brief Source code location of a parsed XML node.
 */
struct XMLNodeLocation
{
   /*!
    * Zero-based text line number where a parsed node has been identified in
    * an XML document, or -1 if text location information is not available.
    */
   int64 line = 0;

   /*!
    * Zero-based text column number, counted from the starting character of a
    * text line, where a parsed node has been identified in an XML document.
    * This member is -1 if text location information is not available.
    *
    * Note that the value stored in this field is actually a character index,
    * not necessarily a valid text column number. It is an actual column number
    * only if the corresponding line of text does not contain tabulator
    * characters (#x9). If there are tabulators, there is usually no one-to-one
    * correspondence between characters and represented text columns.
    */
   int64 column = 0;

   /*!
    * Default constructor. Initializes the line and column members to -1,
    * signaling an undefined source code location.
    */
   XMLNodeLocation() = default;

   /*!
    * Copy constructor.
    */
   XMLNodeLocation( const XMLNodeLocation& ) = default;
};

/*!
 * \class XMLNode
 * \brief Abstract base class of all XML document node classes.
 *
 * XML nodes can be elements, text, CDATA sections, processing instructions
 * (PI), comments, and unknown special elements. This class extends the
 * XMLComponent root class to implement XML document node classification and
 * serialization.
 */
class PCL_CLASS XMLNode : public XMLComponent
{
public:

   /*!
    * Represents the type of an XML document node. Supported/valid node types
    * are defined in the XMLNodeType namespace.
    */
   typedef XMLNodeType::mask_type   node_type;

   /*!
    * Default constructor. Constructs a default %XMLNode object of the
    * specified \a type, with no parent element and undefined source code
    * location.
    */
   XMLNode( node_type type ) :
      XMLComponent(), m_type( type ), m_location()
   {
   }

   /*!
    * Copy constructor.
    *
    * The newly constructed node will be an \e orphan object, that is, it will
    * have no parent element even if the source object \a x is a child node.
    * This reflects the fact that document nodes are unique objects.
    */
   XMLNode( const XMLNode& x ) :
      XMLComponent(), m_type( x.NodeType() ), m_location( x.m_location )
   {
   }

   /*!
    * Returns true iff this is a child node of an existing XML element.
    */
   bool IsChildNode() const
   {
      return (m_type & XMLNodeType::ChildNode) != 0;
   }

   /*!
    * Returns the type of this XML document node.
    */
   node_type NodeType() const
   {
      return node_type( m_type & ~XMLNodeType::ChildNode );
   }

   /*!
    * Returns true iff this node is an XML element.
    */
   bool IsElement() const
   {
      return NodeType() == XMLNodeType::Element;
   }

   /*!
    * Returns a reference to the (immutable) source code location of this node.
    */
   const XMLNodeLocation& Location() const
   {
      return m_location;
   }

   /*!
    * Serializes this document node as an XML fragment encoded in UTF-8.
    *
    * \param text          Reference to an 8-bit string where the UTF-8-encoded
    *                      serialization of this node must be appended.
    *
    * \param indentSize    Number of white space characters to be used for
    *                      indentation of source code lines. A value of zero
    *                      effectively disables indentation.
    *
    * \param level         Recursion level. A value greater than zero denotes
    *                      that this function is being called from a parent
    *                      XML element.
    */
   virtual void Serialize( IsoString& text, int indentSize, int level ) const = 0;

   /*!
    * Returns true iff a new line character (#x0A) can be inserted before
    * serializing this node after the specified \a previous node.
    */
   virtual bool NLAfter( const XMLNode& previous ) const;

private:

   unsigned        m_type;
   XMLNodeLocation m_location;

   friend class XMLDocument;
   friend class XMLElement;
};

/*!
 * \class XMLNodeList
 * \brief Dynamic list of XML node objects.
 *
 * %XMLNodeList is used as the internal implementation of element child node
 * lists and document node lists. In current PCL versions, %XMLNodeList is a
 * template instantiation of ReferenceArray<> for the XMLNode class.
 */
typedef ReferenceArray<XMLNode>  XMLNodeList;

/*!
 * \class XMLAttribute
 * \brief XML element attribute.
 *
 * The %XMLAttribute class represents an element attribute, as defined by the
 * Attribute construct:
 *
 * https://www.w3.org/TR/xml11/#NT-Attribute
 */
class PCL_CLASS XMLAttribute : public XMLComponent
{
public:

   /*!
    * Constructs a new %XMLAttribute object with the specified qualified
    * \a name and \a value.
    *
    * The specified \a name should be a valid XML qualified name, as defined by
    * the W3C recommendation:
    *
    * https://www.w3.org/TR/xml-names/#ns-qualnames
    *
    * However, the \a name is not checked for validity by this constructor, for
    * performance reasons. Attribute and element names are verified during the
    * document parsing and generation tasks.
    */
   XMLAttribute( const String& name, const String& value = String() ) :
      XMLComponent(),
      m_name( name ),
      m_value( value )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLAttribute( const XMLAttribute& ) = default;

   /*!
    * Returns a reference to the (immutable) qualified name of this attribute.
    */
   const String& Name() const
   {
      return m_name;
   }

   /*!
    * Returns a reference to the (immutable) value of this element attribute.
    */
   const String& Value() const
   {
      return m_value;
   }

   /*!
    * Sets a new value for this XML element attribute.
    */
   void SetValue( const String& text )
   {
      m_value = text;
   }

   /*!
    * Returns an encoded version of the attribute value. All characters that
    * cannot legally occur in an XML attribute value are replaced by their
    * corresponding entity references.
    */
   String EncodedValue() const
   {
      return XML::EncodedText( m_value, false/*apos*/ );
   }

   /*!
    * Equality operator.
    *
    * Two XML element attributes are considered equal if their qualified names
    * are identical. Note that this restricts valid attribute comparisons to a
    * particular XML document.
    */
   bool operator ==( const XMLAttribute& x ) const
   {
      return m_name == x.m_name;
   }

   /*!
    * Less-than relational operator.
    *
    * To compare XML element attributes, only their qualified names are taken
    * into account. Note that this restricts valid attribute comparisons to a
    * particular XML document.
    */
   bool operator <( const XMLAttribute& x ) const
   {
      return m_name < x.m_name;
   }

private:

   String m_name;
   String m_value;
};

/*!
 * \class XMLAttributeList
 * \brief Dynamic list of XML element attributes.
 *
 * %XMLAttributeList represents a sequence of XML element attributes in a
 * start-tag, as defined in the W3C recommendation:
 *
 * https://www.w3.org/TR/xml11/#sec-starttags
 *
 * %XMLAttributeList is internally implemented as a dynamic array of
 * XMLAttribute objects.
 */
class PCL_CLASS XMLAttributeList
{
public:

   /*!
    * Represents the dynamic container class used internally to implement an
    * XML element attribute list.
    */
   typedef Array<XMLAttribute>                  list_implementation;

   /*!
    * Represents a mutable XML element attribute list iterator.
    */
   typedef list_implementation::iterator        iterator;

   /*!
    * Represents an immutable XML element attribute list iterator.
    */
   typedef list_implementation::const_iterator  const_iterator;

   /*!
    * Constructs a new %XMLAttributeList object by parsing the specified
    * \a text string.
    *
    * The specified \a text must be a sequence of zero or more Attribute XML
    * definitions pertaining to a start-tag:
    *
    * https://www.w3.org/TR/xml11/#NT-STag
    *
    * See the Parse() member function for a more detailed description.
    */
   XMLAttributeList( const String& text )
   {
      Parse( text );
   }

   /*!
    * Default constructor. Constructs an empty XML attribute list.
    */
   XMLAttributeList() = default;

   /*!
    * Copy constructor.
    */
   XMLAttributeList( const XMLAttributeList& ) = default;

   /*!
    * Returns the number of element attributes in this list.
    */
   int Length() const
   {
      return int( m_list.Length() );
   }

   /*!
    * Returns true iff this attribute list is empty.
    */
   bool IsEmpty() const
   {
      return m_list.IsEmpty();
   }

   /*!
    * Returns a reference to the immutable element attribute at the specified
    * zero-based index \a i. No bounds checking is performed: if the specified
    * index is invalid this function invokes undefined behavior.
    */
   const XMLAttribute& operator []( int i ) const
   {
      return m_list[i];
   }

   /*!
    * Returns an immutable iterator located at the beginning of this element
    * attribute list.
    */
   const_iterator Begin() const
   {
      return m_list.Begin();
   }

   /*!
    * Returns an immutable iterator located at the end of this element
    * attribute list.
    */
   const_iterator End() const
   {
      return m_list.End();
   }

#ifndef __PCL_NO_STL_COMPATIBLE_ITERATORS
   /*!
    * STL-compatible iteration. Equivalent to Begin() const.
    */
   const_iterator begin() const
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to End() const.
    */
   const_iterator end() const
   {
      return End();
   }
#endif   // !__PCL_NO_STL_COMPATIBLE_ITERATORS

   /*!
    * Returns true iff this list contains an element attribute with the
    * specified qualified \a name.
    */
   bool HasAttribute( const String& name ) const
   {
      return m_list.Contains( name );
   }

   /*!
    * Returns the value of the element attribute with the specified qualified
    * \a name, or an empty string if this list does not contain such an element
    * attribute.
    */
   String AttributeValue( const String& name ) const
   {
      const_iterator a = m_list.Search( name );
      return (a != m_list.End()) ? a->Value() : String();
   }

   /*!
    * Causes this list to contain an XML element attribute with the specified
    * qualified \a name and \a value.
    *
    * If an attribute with the same qualified \a name already exists in this
    * list, then its value will be changed. Otherwise, a new attribute will be
    * appended to this list.
    *
    * This member function ensures that no XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * XML specification:
    *
    * https://www.w3.org/TR/xml11/#sec-starttags
    * https://www.w3.org/TR/xml-names/#scoping-defaulting
    */
   void SetAttribute( const String& name, const String& value )
   {
      iterator a = m_list.Search( name );
      if ( a == m_list.End() )
         m_list.Add( XMLAttribute( name, value ) );
      else
         a->SetValue( value );
   }

   /*!
    * Causes this list to contain the specified XML element \a attribute.
    *
    * See SetAttribute( const String&, const String& ) for more information.
    */
   void SetAttribute( const XMLAttribute& attribute )
   {
      iterator a = m_list.Search( attribute );
      if ( a == m_list.End() )
         m_list.Add( attribute );
      else
         *a = attribute;
   }

   /*!
    * Causes this list to contain the specified \a list of XML element
    * attributes.
    *
    * For each attribute in the specified \a list, if an attribute with the
    * same qualified \a name already exists in this list, then its value will
    * be changed. Otherwise, a new attribute will be appended to this list.
    *
    * This member function ensures that no XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * XML specification:
    *
    * https://www.w3.org/TR/xml11/#sec-starttags
    * https://www.w3.org/TR/xml-names/#scoping-defaulting
    */
   void SetAttributes( const XMLAttributeList& list )
   {
      for ( auto a : list )
         SetAttribute( a );
   }

   /*!
    * Removes the element attribute with the specified qualified \a name, if it
    * exists in this list. If no attribute with the specified \a name exists,
    * this member function has no effect.
    */
   void RemoveAttribute( const String& name )
   {
      iterator a = m_list.Search( name );
      if ( a != m_list.End() )
         m_list.Remove( a );
   }

   /*!
    * Removes all element attributes in this list, yielding an empty element
    * attribute list.
    */
   void Clear()
   {
      m_list.Clear();
   }

   /*!
    * Sorts the element attributes in this list in ascending order by comparing
    * their qualified names.
    */
   void Sort()
   {
      m_list.Sort();
   }

   /*!
    * Parses the specified \a text, encoded as UTF-16, to generate a new list
    * of XML element attributes.
    *
    * The specified \a text must be a sequence of zero or more Attribute XML
    * definitions pertaining to a start-tag, as described in the W3C
    * recommendation:
    *
    * https://www.w3.org/TR/xml11/#NT-STag
    *
    * Attribute value normalization is applied to each parsed attribute:
    *
    * https://www.w3.org/TR/xml11/#AVNormalize
    *
    * In attribute values, all entity and character references are decoded. See
    * the XML::DecodedText() static function for more information on reference
    * decoding. Normalization also implies space trimming and compression: all
    * leading and trailing space characters are removed, and all sequences of
    * one or more space characters are replaced by single white space
    * characters (#x20).
    */
   void Parse( const String& text );

   /*!
    * Performs the XML serialization of this element attribute list and appends
    * it to the specified \a text string, encoded in UTF-8.
    *
    * The generated serialization is a sequence of zero or more Attribute XML
    * definitions pertaining to a start-tag, as described in the W3C
    * recommendation:
    *
    * https://www.w3.org/TR/xml11/#NT-STag
    */
   void Serialize( IsoString& text ) const;

private:

   list_implementation m_list;
};

/*!
 * \class XMLElement
 * \brief XML element.
 *
 * The %XMLElement class represents an XML document element:
 *
 * https://www.w3.org/TR/xml11/#dt-element
 *
 * Elements are the main data holders in the logical design of XML, following a
 * hierarchical tree structure.
 */
class PCL_CLASS XMLElement : public XMLNode
{
public:

   /*!
    * Represents a mutable child node list iterator.
    */
   typedef XMLNodeList::iterator          iterator;

   /*!
    * Represents an immutable child node list iterator.
    */
   typedef XMLNodeList::const_iterator    const_iterator;

   /*!
    * Default constructor. Constructs an uninitialized %XMLElement structure.
    */
   XMLElement() :
      XMLNode( XMLNodeType::Element ),
      m_childTypes( XMLNodeType::Undefined )
   {
   }

   /*!
    * Constructs an empty %XMLElement object with the specified qualified
    * \a name and \a attributes.
    */
   XMLElement( const String& name, const XMLAttributeList& attributes = XMLAttributeList() ) :
      XMLNode( XMLNodeType::Element ),
      m_name( name ),
      m_attributes( attributes ),
      m_childTypes( XMLNodeType::Undefined )
   {
   }

   XMLElement( const XMLElement& ) = delete;
   XMLElement& operator =( const XMLElement& ) = delete;

   /*!
    * Virtual destructor. If this element contains child nodes, all of them
    * will be destroyed recursively.
    */
   virtual ~XMLElement()
   {
      DestroyChildNodes();
   }

   /*!
    * Returns true iff this is a root XML element. A root XML element has no
    * parent element.
    *
    * Note that this member function can return true in two different
    * situations: when the element has been generated during a document parsing
    * process (in which case this is an actual document root element), and if
    * this object has not been initialized yet (because it has been newly
    * constructed and still has not been associated with an XML document).
    */
   bool IsRootElement() const
   {
      return ParentElement() == nullptr;
   }

   /*!
    * Returns a reference to the (immutable) qualified element name.
    */
   const String& Name() const
   {
      return m_name;
   }

   /*!
    * Returns a copy of the list of XML element attributes.
    */
   XMLAttributeList Attributes() const
   {
      return m_attributes;
   }

   /*!
    * Returns true iff this element has one or more attributes defined.
    */
   bool HasAttributes() const
   {
      return !m_attributes.IsEmpty();
   }

   /*!
    * Returns true iff this element has an attribute with the specified
    * qualified \a name.
    */
   bool HasAttribute( const String& name ) const
   {
      return m_attributes.HasAttribute( name );
   }

   /*!
    * Returns the value of the attribute with the specified qualified \a name
    * in this element, or an empty string if this element has no such
    * attribute.
    */
   String AttributeValue( const String& name ) const
   {
      return m_attributes.AttributeValue( name );
   }

   /*!
    * Causes this XML element to contain an attribute with the specified
    * qualified \a name and \a value.
    *
    * If an attribute with the same qualified \a name already exists in this
    * element, then its value will be changed. Otherwise, a new attribute will
    * be created in this element.
    *
    * This member function ensures that no XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * XML specification:
    *
    * https://www.w3.org/TR/xml11/#sec-starttags
    * https://www.w3.org/TR/xml-names/#scoping-defaulting
    */
   void SetAttribute( const String& name, const String& value )
   {
      XMLAttribute a( name, value );
      a.m_parent = this;
      m_attributes.SetAttribute( a );
   }

   /*!
    * Causes this XML element to contain the specified \a attribute.
    *
    * See SetAttribute( const String&, const String& ) for more information.
    */
   void SetAttribute( const XMLAttribute& attribute )
   {
      XMLAttribute a( attribute );
      a.m_parent = this;
      m_attributes.SetAttribute( a );
   }

   /*!
    * Causes this XML element to contain the specified \a list of attributes.
    *
    * For each attribute in the specified \a list, if an attribute with the
    * same qualified \a name already exists in this element, then its value
    * will be changed. Otherwise, a new attribute will be created in this
    * element.
    *
    * This member function ensures that no XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * XML specification:
    *
    * https://www.w3.org/TR/xml11/#sec-starttags
    * https://www.w3.org/TR/xml-names/#scoping-defaulting
    */
   void SetAttributes( const XMLAttributeList& list )
   {
      for ( auto a : list )
         SetAttribute( a );
   }

   /*!
    * Removes the attribute with the specified qualified \a name, if it exists
    * in this element. If this element has no attribute with the specified
    * \a name, this member function has no effect.
    */
   void RemoveAttribute( const String& name )
   {
      m_attributes.RemoveAttribute( name );
   }

   /*!
    * Removes all existing attributes in this element.
    */
   void ClearAttributes()
   {
      m_attributes.Clear();
   }

   /*!
    * Sorts the existing attributes in this element in ascending order by
    * comparing their qualified names.
    */
   void SortAttributes()
   {
      m_attributes.Sort();
   }

   /*!
    * Sorts the existing attributes in this element in ascending order.
    * Ordering of elements is defined such that for any pair a, b of
    * XMLAttribute objects in this element, the binary predicate p(a,b) is true
    * iff a precedes b.
    */
   template <class BP>
   void SortAttributes( BP p )
   {
      m_attributes.Sort( p );
   }

   /*!
    * Parses the specified \a text, encoded as UTF-16, to generate a new list
    * of attributes in this XML element. The previous list of attributes, if
    * any, will be replaced by the newly generated list.
    *
    * The specified \a text must be a sequence of zero or more Attribute XML
    * definitions pertaining to a start-tag, as described in the W3C
    * recommendation:
    *
    * https://www.w3.org/TR/xml11/#NT-STag
    */
   void ParseAttributes( const String& text )
   {
      XMLAttributeList list( text );
      ClearAttributes();
      SetAttributes( list );
   }

   /*!
    * Performs the XML serialization of the attribute list in this element and
    * appends it to the specified \a text string, encoded in UTF-8.
    *
    * The generated serialization is a sequence of zero or more Attribute XML
    * definitions pertaining to a start-tag, as described in the W3C
    * recommendation:
    *
    * https://www.w3.org/TR/xml11/#NT-STag
    */
   void SerializeAttributes( IsoString& text ) const
   {
      m_attributes.Serialize( text );
   }

   /*!
    * Returns the number of child nodes in this XML element.
    */
   int ChildCount() const
   {
      return int( m_childNodes.Length() );
   }

   /*!
    * Returns true iff this is an empty XML element. An empty element has no
    * child nodes.
    */
   bool IsEmpty() const
   {
      return m_childNodes.IsEmpty();
   }

   /*!
    * Returns a reference to the immutable child node at the specified
    * zero-based index \a i. No bounds checking is performed: if the specified
    * index is invalid this function invokes undefined behavior.
    */
   const XMLNode& operator []( int i ) const
   {
      return m_childNodes[i];
   }

   /*!
    * Returns a reference to the immutable first child node in this element. No
    * bounds checking is performed: if this element is empty, this function
    * invokes undefined behavior.
    */
   const XMLNode& First() const
   {
      return m_childNodes.First();
   }

   /*!
    * Returns a reference to the immutable last child node in this element. No
    * bounds checking is performed: if this element is empty, this function
    * invokes undefined behavior.
    */
   const XMLNode& Last() const
   {
      return m_childNodes.Last();
   }

   /*!
    * Returns an immutable iterator located at the beginning of the list of
    * child nodes of this XML element.
    */
   const_iterator Begin() const
   {
      return m_childNodes.Begin();
   }

   /*!
    * Returns an immutable iterator located at the end of the list of child
    * nodes of this XML element.
    */
   const_iterator End() const
   {
      return m_childNodes.End();
   }

#ifndef __PCL_NO_STL_COMPATIBLE_ITERATORS
   /*!
    * STL-compatible iteration. Equivalent to Begin() const.
    */
   const_iterator begin() const
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to End() const.
    */
   const_iterator end() const
   {
      return End();
   }
#endif   // !__PCL_NO_STL_COMPATIBLE_ITERATORS

   /*!
    * Returns true iff this element contains one or more child XML elements.
    */
   bool HasElements() const
   {
      return (m_childTypes & XMLNodeType::Element) != 0;
   }

   /*!
    * Returns true iff this element contains one or more child text blocks.
    */
   bool HasText() const
   {
      return (m_childTypes & XMLNodeType::Text) != 0;
   }

   /*!
    * Returns true iff this element contains one or more child CDATA sections.
    */
   bool HasCDATA() const
   {
      return (m_childTypes & XMLNodeType::CDATA) != 0;
   }

   /*!
    * Returns true iff this element contains one or more child processing
    * instructions.
    */
   bool HasProcessingInstructions() const
   {
      return (m_childTypes & XMLNodeType::ProcessingInstructions) != 0;
   }

   /*!
    * Returns true iff this element contains one or more child comment
    * sections.
    */
   bool HasComments() const
   {
      return (m_childTypes & XMLNodeType::Comment) != 0;
   }

   /*!
    * \internal
    */
   void GetChildElementsByName_Recursive( XMLNodeList& list, const String& name ) const
   {
      for ( const XMLNode& node : m_childNodes )
         if ( node.IsElement() )
            if ( static_cast<const XMLElement&>( node ).Name() == name )
            {
               list << &node;
               static_cast<const XMLElement&>( node ).GetChildElementsByName_Recursive( list, name );
            }
   }

   /*!
    * Returns a list with all child elements of this element with the specified
    * \a name.
    *
    * This member function performs a recursive search across the entire tree
    * structure rooted at this element.
    */
   XMLNodeList ChildElementsByName( const String& name ) const
   {
      XMLNodeList list;
      GetChildElementsByName_Recursive( list, name );
      return list;
   }

   /*!
    * \internal
    */
   void GetChildNodesByType_Recursive( XMLNodeList& list, node_type type ) const
   {
      for ( const XMLNode& node : m_childNodes )
         if ( (node.NodeType() & type) != 0 )
         {
            list << &node;
            if ( node.IsElement() )
               static_cast<const XMLElement&>( node ).GetChildNodesByType_Recursive( list, type );
         }
   }

   /*!
    * Returns a list with all child nodes of this element of the specified
    * \a type. The \a type argument can be an ORed combination of XMLNodeType
    * enumerated mask values.
    *
    * This member function performs a recursive search across the entire tree
    * structure rooted at this element.
    */
   XMLNodeList ChildNodesByType( node_type type ) const
   {
      XMLNodeList list;
      GetChildNodesByType_Recursive( list, type );
      return list;
   }

   /*!
    * \internal
    */
   template <class UP>
   void ChildNodesThat_Recursive( XMLNodeList& list, UP u ) const
   {
      for ( const XMLNode& node : m_childNodes )
         if ( u( node ) )
         {
            list << &node;
            if ( node.IsElement() )
               static_cast<const XMLElement&>( node ).ChildNodesThat_Recursive( list, u );
         }
   }

   /*!
    * Returns a list with all child nodes of this element that satisfy the
    * specified unary predicate \a u.
    *
    * For each child node n in this element, n will be included in the returned
    * list iff u( n ) returns true.
    *
    * This member function performs a recursive search across the entire tree
    * structure rooted at this element.
    */
   template <class UP>
   XMLNodeList ChildNodesThat( UP u ) const
   {
      XMLNodeList list;
      ChildNodesThat_Recursive( list, u );
      return list;
   }

   /*!
    * Appends a new child \a node to this XML element.
    *
    * The specified \a node will be owned by this element, which will destroy
    * it automatically (and recursively) upon destruction.
    */
   void AddChildNode( XMLNode* node )
   {
      m_childNodes << node;
      node->m_parent = this;
      node->m_type |= XMLNodeType::ChildNode;
      m_childTypes |= node->NodeType();
   }

   /*!
    * \internal
    * Appends a new child \a node to this XML element.
    *
    * The specified \a node will be owned by this element, which will destroy
    * it automatically (and recursively) upon destruction.
    */
   void AddChildNode( XMLNode* node, const XMLNodeLocation& location )
   {
      node->m_location = location;
      AddChildNode( node );
   }

   /*!
    * Recursively destroys all existing child nodes in this XML element,
    * yielding an empty element.
    */
   void DestroyChildNodes()
   {
      m_childNodes.Destroy();
      m_childTypes = XMLNodeType::Undefined;
   }

   /*!
    * Recursively serializes this XML element and its contents. Appends the
    * generated XML source code to the specified 8-bit \a text string, encoded
    * in UTF-8.
    */
   virtual void Serialize( IsoString& text, int indentSize, int level ) const;

private:

   String           m_name;
   XMLAttributeList m_attributes;
   XMLNodeList      m_childNodes;
   unsigned         m_childTypes;
};

/*!
 * \class XMLText
 * \brief XML text block.
 *
 * This %XMLText class represents a text entity in an XML document:
 *
 * https://www.w3.org/TR/xml11/#dt-text
 */
class PCL_CLASS XMLText : public XMLNode
{
public:

   /*!
    * Constructs a new %XMLText object.
    *
    * \param text    The Unicode text block contents encoded as UTF-16.
    *
    * \param preserveSpaces   If false, the text block will be transformed by
    *                trimming and collapsing spaces: All leading and trailing
    *                space characters will be removed, and all sequences of one
    *                or more space characters will be replaced by single white
    *                space characters (#x20). If true, the specified \a text
    *                string will be stored intact.
    *
    * Besides text contents transformation, space preservation also has an
    * impact in the way text blocks are serialized as XML: New line characters
    * (#x0A) are never used to separate text blocks from their parent or
    * sibling nodes when space preservation is enabled.
    */
   XMLText( const String& text, bool preserveSpaces = true ) :
      XMLNode( XMLNodeType::Text ),
      m_text( preserveSpaces ? text : XML::CollapsedSpaces( XML::TrimmedSpaces( text ) ) ),
      m_preserveSpaces( preserveSpaces )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLText( const XMLText& ) = default;

   /*!
    * Returns a reference to the (immutable) text string contained by this XML
    * text block. The returned string is encoded in UTF-16.
    */
   const String& Text() const
   {
      return m_text;
   }

   /*!
    * Returns true iff this text block preserves space characters for
    * serialization. See the class constructor for more information.
    */
   bool IsPreserveSpaces() const
   {
      return m_preserveSpaces;
   }

   /*!
    * Returns an encoded version of this text block. All characters that cannot
    * legally occur in an XML text block are replaced by their corresponding
    * entity references.
    */
   String EncodedText() const
   {
      return XML::EncodedText( m_text );
   }

   /*!
    * Returns a space-transformed version of this text block.
    *
    * \param collapse   Replace all sequences of one or more space characters
    *                   with single white space characters (#x20).
    *
    * \param trim       Remove all leading and trailing space characters.
    */
   String SpaceTransformedText( bool collapse, bool trim ) const
   {
      String text = m_text;
      if ( trim )
         text = XML::TrimmedSpaces( text );
      if ( collapse )
         text = XML::CollapsedSpaces( text );
      return text;
   }

   /*!
    * Serializes this XML text block with UTF-8 encoding.
    */
   virtual void Serialize( IsoString& text, int indentSize, int level ) const;

   /*!
    * Returns true iff a new line character (#x0A) can be inserted before
    * serializing this node after the specified \a previous node.
    *
    * In the case of a text block, a new line character can only be inserted
    * if the block does not preserve space characters. If space preservation is
    * enabled, new line characters are forbidden at the beginning and end of
    * the text block serialization.
    */
   virtual bool NLAfter( const XMLNode& previous ) const
   {
      return !m_preserveSpaces;
   }

private:

   String m_text; // N.B.: This is plain, that is, decoded, text.
   bool   m_preserveSpaces : 1;
};

/*!
 * \class XMLCDATA
 * \brief XML CDATA section.
 *
 * The %XMLCDATA class represents a CDATA section in an XML document:
 *
 * https://www.w3.org/TR/xml11/#sec-cdata-sect
 */
class PCL_CLASS XMLCDATA : public XMLNode
{
public:

   /*!
    * Constructs a new %XMLCDATA object with the specified character \a data
    * encoded in UTF-16.
    *
    * The specified \a data must not contain the sequence "]]>". Any occurrence
    * of this forbidden sequence will be removed for serialization.
    */
   XMLCDATA( const String& data ) :
      XMLNode( XMLNodeType::CDATA ),
      m_cdata( data )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLCDATA( const XMLCDATA& ) = default;

   /*!
    * Returns a reference to the (immutable) character data string, encoded as
    * UTF-16, contained by this CDATA section.
    */
   const String& CData() const
   {
      return m_cdata;
   }

   /*!
    * Serializes this XML CDATA section with UTF-8 encoding.
    */
   virtual void Serialize( IsoString& text, int indentSize, int level ) const;

private:

   String m_cdata;
};

/*!
 * \class XMLProcessingInstructions
 * \brief XML processing instructions.
 *
 * The %XMLProcessingInstructions class represents a processing instructions
 * (PI) tag in an XML document:
 *
 * https://www.w3.org/TR/xml11/#sec-pi
 */
class XMLProcessingInstructions : public XMLNode
{
public:

   /*!
    * Constructs a new %XMLProcessingInstructions object with the specified
    * \a target name and \a instructions string, both encoded in UTF-16.
    *
    * The specified \a instructions string must not contain the sequence "?>".
    * Any occurrence of this forbidden sequence will be removed for
    * serialization.
    */
   XMLProcessingInstructions( const String& target, const String& instructions ) :
      XMLNode( XMLNodeType::ProcessingInstructions ),
      m_target( target ),
      m_instructions( instructions )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLProcessingInstructions( const XMLProcessingInstructions& ) = default;

   /*!
    * Returns a reference to the (immutable) instructions target name.
    */
   const String& Target() const
   {
      return m_target;
   }

   /*!
    * Returns a reference to the (immutable) instructions string.
    */
   const String& Instructions() const
   {
      return m_instructions;
   }

   /*!
    * Serializes this XML PI section with UTF-8 encoding.
    */
   virtual void Serialize( IsoString& text, int indentSize, int level ) const;

private:

   String m_target;
   String m_instructions;
};

/*!
 * \class XMLComment
 * \brief XML comment section.
 *
 * The %XMLComment class represents a comment in an XML document:
 *
 * https://www.w3.org/TR/xml11/#sec-comments
 */
class PCL_CLASS XMLComment : public XMLNode
{
public:

   /*!
    * Constructs a new %XMLComment object with the specified \a comment string
    * encoded in UTF-16.
    *
    * The specified \a comment must not contain the sequence "--" or end with
    * a '-' character. Any occurrence of these forbidden sequences will be
    * removed for serialization.
    */
   XMLComment( const String& comment ) :
      XMLNode( XMLNodeType::Comment ),
      m_comment( comment )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLComment( const XMLComment& ) = default;

   /*!
    * Returns a reference to the (immutable) comment string.
    */
   const String& Comment() const
   {
      return m_comment;
   }

   /*!
    * Serializes this XML comment section with UTF-8 encoding.
    */
   virtual void Serialize( IsoString& text, int indentSize, int level ) const;

private:

   String m_comment;
};

/*!
 * \class XMLUnknownElement
 * \brief Unsupported or invalid XML element.
 *
 * %XMLUnknownElement represents an invalid or unrecognized XML element
 * retrieved while parsing an XML document. In the current PCL implementation,
 * an %XMLUnknownElement object is generated if the parser finds an element
 * whose start-tag begins with the "<!" token and is neither a comment section
 * nor a DOCTYPE declaration.
 */
class PCL_CLASS XMLUnknownElement : public XMLNode
{
public:

   /*!
    * Constructs an %XMLUnknownElement with the specified qualified \a name and
    * element \a parameters.
    */
   XMLUnknownElement( const String& name, const String& parameters = String() ) :
      XMLNode( XMLNodeType::Unknown ),
      m_name( name ),
      m_parameters( parameters )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLUnknownElement( const XMLUnknownElement& ) = default;

   /*!
    * Returns a reference to the (immutable) unknown element name.
    */
   const String& Name() const
   {
      return m_name;
   }

   /*!
    * Returns a reference to the (immutable) unknown element parameters.
    */
   const String& Parameters() const
   {
      return m_parameters;
   }

   /*!
    * Serializes this XML unknown element with UTF-8 encoding.
    */
   virtual void Serialize( IsoString& text, int indentSize, int level ) const;

private:

   String m_name;
   String m_parameters;
};

/*!
 * \class XMLDeclaration
 * \brief XML declaration.
 *
 * %XMLDeclaration represents an XML declaration in an XML document prolog:
 *
 * https://www.w3.org/TR/xml11/#sec-prolog-dtd
 */
class PCL_CLASS XMLDeclaration
{
public:

   /*!
    * Constructs a new %XMLDeclaration object with the specified \a version,
    * optional \a encoding and \a standalone document specification.
    */
   XMLDeclaration( const String& version = String(), const String& encoding = String(), bool standalone = false ) :
      m_version( version ),
      m_encoding( encoding ),
      m_standalone( standalone )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLDeclaration( const XMLDeclaration& ) = default;

   /*!
    * Returns a reference to the (immutable) XML version string.
    */
   const String& Version() const
   {
      return m_version;
   }

   /*!
    * Returns a reference to the (immutable) document encoding string.
    */
   const String& DocumentEncoding() const
   {
      return m_encoding;
   }

   /*!
    * Returns true iff this XML declaration specifies a standalone document.
    */
   bool IsStandaloneDocument() const
   {
      return m_standalone;
   }

   /*!
    * Returns true iff this XML declaration has been defined. This function can
    * be used to check if a parsed XML document includes an XML declaration.
    */
   bool IsDefined() const
   {
      return !m_version.IsEmpty();
   }

   /*!
    * Serializes this XML declaration. If no version string has been defined,
    * a 'version="1.0"' attribute will be generated. Similarly, if no encoding
    * string has been specified, a 'encoding="UTF-8"' attribute will be
    * generated.
    *
    * The generated serialization will be appended to the specified 8-bit
    * \a text string.
    */
   void Serialize( IsoString& text, int indentSize, int level ) const;

private:

   String m_version;
   String m_encoding;
   bool   m_standalone : 1;
};

/*!
 * \class XMLDocTypeDeclaration
 * \brief XML DOCTYPE declaration.
 *
 * %XMLDocTypeDeclaration represents a document type declaration in an XML
 * document prolog:
 *
 * https://www.w3.org/TR/xml11/#dt-doctype
 */
class PCL_CLASS XMLDocTypeDeclaration
{
public:

   /*!
    * Constructs a new %XMLDocTypeDeclaration object with the specified
    * document type \a name and type \a definition.
    */
   XMLDocTypeDeclaration( const String& name = String(), const String& definition = String() ) :
      m_name( name ),
      m_definition( definition )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLDocTypeDeclaration( const XMLDocTypeDeclaration& ) = default;

   /*!
    * Returns a reference to the (immutable) document type name.
    */
   const String& Name() const
   {
      return m_name;
   }

   /*!
    * Returns a reference to the (immutable) document type definition.
    */
   const String& Definition() const
   {
      return m_definition;
   }

   /*!
    * Returns true iff this document type declaration has been defined. This
    * function can be used to check if a parsed XML document includes an
    * DOCTYPE declaration.
    */
   bool IsDefined() const
   {
      return !m_name.IsEmpty();
   }

   /*!
    * Serializes this DOCTYPE declaration.
    */
   void Serialize( IsoString& text, int indentSize, int level ) const;

private:

   String m_name;
   String m_definition;
};

/*!
 * \class XMLDocument
 * \brief XML document parsing and generation.
 *
 * %XMLDocument implements parsing and generation of well-formed XML documents.
 *
 * The Parse() member function reads and interprets a Unicode text string to
 * generate a read-only document object model (DOM) that represents the data
 * entities defined by an XML document. The DOM can then be inspected with
 * several member functions of the %XMLDocument class. All XML nodes and
 * elements in a document can be visited recursively with specialized accessor
 * functions and iterators. See the Begin() and End() functions (and their
 * STL-compatible equivalents, begin() and end()), as well as XML(), DocType(),
 * RootElement(), and operator []( int ), among others.
 *
 * For generation of well-formed XML documents, the Serialize() member function
 * builds a new XML document as a Unicode string encoded in UTF-8. The
 * document's root node and several nodes and critical components must be
 * defined before document generation - see the SetXML(), SetDocType(),
 * AddNode() and SetRootElement() member functions.
 *
 * For general information on XML, the authoritative sources are the W3C
 * recommendations:
 *
 * https://www.w3.org/TR/xml/
 * https://www.w3.org/TR/xml11/
 * https://www.w3.org/TR/xml-names/
 *
 * The following example shows how an existing document can be parsed as a new
 * XMLDocument object, and then a new XML document can be generated and written
 * to a disk file, all in just three source code lines:
 *
 * \code
 * XMLDocument xml;
 * xml.Parse( File::ReadTextFile( "/path/to/file.xml" ).UTF8ToUTF16() );
 * File::WriteTextFile( "/tmp/test.xml", xml.Serialize( 3 ) );
 * \endcode
 */
class PCL_CLASS XMLDocument
{
public:

   /*!
    * Represents a mutable child node list iterator.
    */
   typedef XMLNodeList::iterator          iterator;

   /*!
    * Represents an immutable child node list iterator.
    */
   typedef XMLNodeList::const_iterator    const_iterator;

   /*!
    * Default constructor. Constructs an empty XML document.
    */
   XMLDocument() = default;

   XMLDocument( const XMLDocument& ) = delete;
   XMLDocument& operator =( const XMLDocument& ) = delete;

   /*!
    * Virtual destructor. Recursively destroys all XML elements, declarations
    * and auxiliary data associated with this object.
    */
   virtual ~XMLDocument()
   {
      m_nodes.Destroy();
      m_root = nullptr;
   }

   /*!
    * Returns a reference to the (immutable) XML declaration object associated
    * with this document.
    */
   const XMLDeclaration& XML() const
   {
      return m_xml;
   }

   /*!
    * Associates a new XML declaration object with this XML document.
    */
   void SetXML( const XMLDeclaration& xml )
   {
      m_xml = xml;
   }

   /*!
    * Returns a reference to the (immutable) XML document type declaration
    * object associated with this document.
    */
   const XMLDocTypeDeclaration& DocType() const
   {
      return m_docType;
   }

   /*!
    * Associates a new XML document type declaration object with this XML
    * document.
    */
   void SetDocType( const XMLDocTypeDeclaration& docType )
   {
      m_docType = docType;
   }

   /*!
    * Returns a pointer to the (immutable) root element of this XML document.
    * If there is no root element, for example when this is an uninitialized
    * %XMLDocument instance, this function returns \c nullptr.
    */
   const XMLElement* RootElement() const
   {
      return m_root;
   }

   /*!
    * Returns the number of nodes in this XML document, or zero if this is an
    * empty or uninitialized %XMLDocument object.
    */
   int NodeCount() const
   {
      return int( m_nodes.Length() );
   }

   /*!
    * Returns true iff this is an empty XML document. An empty document has no
    * XML nodes.
    */
   bool IsEmpty() const
   {
      return m_nodes.IsEmpty();
   }

   /*!
    * Returns a reference to the (immutable) document node at the specified
    * zero-based index \a i. No bounds checking is performed: if the specified
    * index is invalid, this function invokes undefined behavior.
    */
   const XMLNode& operator []( int i ) const
   {
      return m_nodes[i];
   }

   /*!
    * Returns an immutable iterator located at the beginning of the list of
    * nodes of this XML document.
    */
   const_iterator Begin() const
   {
      return m_nodes.Begin();
   }

   /*!
    * Returns an immutable iterator located at the end of the list of
    * nodes of this XML document.
    */
   const_iterator End() const
   {
      return m_nodes.End();
   }

#ifndef __PCL_NO_STL_COMPATIBLE_ITERATORS
   /*!
    * STL-compatible iteration. Equivalent to Begin() const.
    */
   const_iterator begin() const
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to End() const.
    */
   const_iterator end() const
   {
      return End();
   }
#endif   // !__PCL_NO_STL_COMPATIBLE_ITERATORS

   /*!
    * Appends a new top-level XML node to this document.
    *
    * If the specified \a node already belongs to an %XMLDocument object, or if
    * a null pointer is specified, this member function will throw an Error
    * exception.
    *
    * The specified \a node will be appended to the current list of document
    * nodes. If there is a root element in this document, the new \a node will
    * be appended after the root element.
    *
    * The \a node will be owned by this document object, which will destroy and
    * deallocate it automatically when appropriate.
    */
   void AddNode( XMLNode* node );

   /*!
    * Sets the root element of this XML document.
    *
    * If the specified \a element already belongs to an %XMLDocument object, if
    * a null pointer is specified, or if a root node has already been defined
    * for this document, this member function will throw an Error exception.
    *
    * The specified \a element will be appended to the current list of document
    * nodes. The \a element will be owned by this document object, which will
    * destroy and deallocate it automatically when appropriate.
    */
   void SetRootElement( XMLElement* element );

   /*!
    * Destroys and deallocates all nodes and elements in this XML document
    * object, and initializes all internal structures to a default state,
    * yielding an uninitialized object.
    */
   void Clear();

   /*!
    * XML document parser. Reads and interprets the specified Unicode \a text
    * string, which must be encoded in UTF-16, as a well-formed XML document.
    *
    * This member function generates a document object model (DOM) to represent
    * the data entities defined by the source XML document. The DOM can then be
    * inspected with several member functions of the %XMLDocument class. All
    * XML nodes and elements can be visited recursively with specialized
    * iterators. See the Begin() and End() functions (and their STL-compatible
    * equivalents, begin() and end()), as well as XML(), DocType(),
    * RootElement() and operator []( int ), among others.
    */
   void Parse( const String& text );

   /*!
    * Serializes this XML document. Returns the generated serialization as an
    * 8-bit string encoded in UTF-8.
    *
    * To serialize a well-formed XML document, this object must be initialized
    * first by defining a root element (see SetRootElement()) and other
    * document nodes, as necessary (see SetXML(), SetDocType(), and AddNode()).
    */
   IsoString Serialize( int indentSize = -1, int level = 0 ) const;

private:

   XMLDeclaration        m_xml;
   XMLDocTypeDeclaration m_docType;
   XMLNodeList           m_nodes;
   XMLElement*           m_root = nullptr;
   XMLNodeLocation       m_location;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_XML_h
