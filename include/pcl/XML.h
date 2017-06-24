//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/XML.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_XML_h
#define __PCL_XML_h

/// \file pcl/XML.h

#include <pcl/Defs.h>

#include <pcl/Exception.h>
#include <pcl/ReferenceArray.h>
#include <pcl/String.h>

#include <errno.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup xml_parsing_and_generation XML Document Parsing and Generation
 */

// ----------------------------------------------------------------------------

class PCL_CLASS XMLDocument;
class PCL_CLASS XMLElement;

// ----------------------------------------------------------------------------

/*!
 * \class XML
 * \brief Utility functions and data for %XML document parsing and generation
 * \ingroup xml_parsing_and_generation
 */
class PCL_CLASS XML
{
public:

   /*!
    * Default constructor. This constructor is disabled because %XML is not an
    * instantiable class.
    */
   XML() = delete;

   /*!
    * Copy constructor. This constructor is disabled because %XML is not an
    * instantiable class.
    */
   XML( const XML& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %XML is not an
    * instantiable class.
    */
   XML& operator =( const XML& ) = delete;

   /*!
    * Destructor. This destructor is disabled because %XML is not an
    * instantiable class.
    */
   ~XML() = delete;

   /*!
    * Returns true iff the specified character \a c is either a white space
    * (\#x20) or a tabulator (#9) character.
    */
   template <typename T>
   static bool IsWhiteSpaceChar( T c )
   {
      return c == 0x20 || c == 9;
   }

   /*!
    * Returns true iff the specified character \a c is either a line feed
    * (\#x0A) or a carriage return (#0D) control character.
    */
   template <typename T>
   static bool IsLineBreakChar( T c )
   {
      return c == 0x0A || c == 0x0D;
   }

   /*!
    * Returns true iff the specified character \a c is an %XML space character:
    *
    * https://www.w3.org/TR/xml11/#NT-S
    */
   template <typename T>
   static bool IsSpaceChar( T c )
   {
      return IsWhiteSpaceChar( c ) || IsLineBreakChar( c );
   }

   /*!
    * Returns true iff the specified character \a c is an %XML NameStartChar:
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
    * Returns true iff the specified character \a c is an %XML NameChar:
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
    * Returns true iff the specified character \a c is an %XML RestrictedChar:
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
    * Returns true iff the specified \a name is a valid %XML qualified element
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
    * with single white space characters (\#x20).
    */
   static String CollapsedSpaces( String::const_iterator i, String::const_iterator j );

   /*!
    * Returns a copy of the specified \a text with all sequences of one or more
    * space characters replaced with single white space characters (\#x20).
    */
   static String CollapsedSpaces( const String& text );

   /*!
    * Returns a copy of the text fragment defined by the range [i,j) of string
    * iterators with all %XML references replaced by their corresponding UTF-16
    * characters.
    *
    * Both entity and character references are decoded by this function. For
    * entity references, the entire set of %XML reference names is supported:
    *
    * http://www.w3.org/TR/xml-entity-names/
    *
    * Character references are interpreted as defined in the %XML
    * specification:
    *
    * https://www.w3.org/TR/xml11/#NT-CharRef
    */
   static String DecodedText( String::const_iterator i, String::const_iterator j );

   /*!
    * Returns a copy of the specified \a text with all %XML references replaced
    * by their corresponding UTF-16 characters.
    *
    * See DecodedText( String::const_iterator, String::const_iterator ) for a
    * detailed description.
    */
   static String DecodedText( const String& text );

   /*!
    * Returns a copy of the text fragment defined by the range [i,j) of string
    * iterators with all occurences of '&', '<', '>' and '"' replaced with the
    * entity references "amp", "lt", "gt" and "quot", respectively. If \a apos
    * is true, single quotes will also be replaced with "apos" entities.
    */
   static String EncodedText( String::const_iterator i, String::const_iterator j, bool apos = true )
   {
      return EncodedText( String( i, j ), apos );
   }

   /*!
    * Returns a copy of the specified \a text with all occurences of '&', '<',
    * '>' and '"' replaced with the entity references "amp", "lt", "gt" and
    * "quot", respectively. If \a apos is true, single quotes will also be
    * replaced with "apos" entities.
    */
   static String EncodedText( const String& text, bool apos = true );

   /*!
    * Returns the Unicode value (encoded as UTF-16) corresponding to an %XML
    * reference defined by the range [i,j) of string iterators:
    *
    * https://www.w3.org/TR/xml11/#NT-Reference
    *
    * Both entity and character references are decoded by this function. For
    * entity references, the entire set of %XML reference names is supported:
    *
    * http://www.w3.org/TR/xml-entity-names/
    *
    * Character references are interpreted as defined in the %XML
    * specification:
    *
    * https://www.w3.org/TR/xml11/#NT-CharRef
    */
   static String ReferenceValue( String::const_iterator i, String::const_iterator j );

   /*!
    * Returns the Unicode value (encoded as UTF-16) corresponding to the
    * specified %XML \a reference.
    *
    * See ReferenceValue( String::const_iterator, String::const_iterator ) for
    * a detailed description.
    */
   static String ReferenceValue( const String& reference )
   {
      return ReferenceValue( reference.Begin(), reference.End() );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLComponent
 * \brief Root base class of all %XML document components
 *
 * %XMLComponent supports the hierarchical structure of an %XML document by
 * implementing the basic concept of <em>parent element</em>.
 *
 * \ingroup xml_parsing_and_generation
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
    * Returns a pointer to the parent %XML element of this component, or
    * \c nullptr if this object has no parent element.
    */
   XMLElement* ParentElement() const
   {
      return m_parent;
   }

   /*!
    * Returns true iff this is a top-level component. Top-level document
    * components have no parent elements.
    */
   bool IsTopLevel() const
   {
      return m_parent == nullptr;
   }

private:

   XMLElement* m_parent = nullptr;

   friend class XMLElement;
};

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::XMLNodeType
 * \brief     %XML document node types
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>XMLNodeType::Undefined</td>              <td>Undefined %XML node type.</td></tr>
 * <tr><td>XMLNodeType::ChildNode</td>              <td>Signals a child %XML document node - for internal use only.</td></tr>
 * <tr><td>XMLNodeType::Unknown</td>                <td>Represents an unsupported %XML node type.</td></tr>
 * <tr><td>XMLNodeType::Element</td>                <td>An %XML element.</td></tr>
 * <tr><td>XMLNodeType::Text</td>                   <td>A text block inside an element's contents.</td></tr>
 * <tr><td>XMLNodeType::CDATA</td>                  <td>A CDATA section.</td></tr>
 * <tr><td>XMLNodeType::ProcessingInstructions</td> <td>A processing instructions (PI) section.</td></tr>
 * <tr><td>XMLNodeType::PI</td>                     <td>A synonym to ProcessingInstructions.</td></tr>
 * <tr><td>XMLNodeType::Comment</td>                <td>A comment block.</td></tr>
 * </table>
 *
 * \ingroup xml_parsing_and_generation
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

   /*!
    * Returns the name of the specified %XML node \a type ("element", "text",
    * "comment", etc).
    */
   String AsString( mask_type type );
}

/*!
 * \class pcl::XMLNodeTypes
 * \brief A collection of %XML node types
 * \ingroup xml_parsing_and_generation
 */
typedef Flags<XMLNodeType::mask_type>  XMLNodeTypes;

// ----------------------------------------------------------------------------

/*!
 * \struct XMLNodeLocation
 * \brief Source code location of a parsed %XML document node
 * \ingroup xml_parsing_and_generation
 */
struct XMLNodeLocation
{
   /*!
    * Zero-based text line number where a parsed node has been identified in
    * an %XML document, or -1 if text location information is not available.
    */
   int64 line = 0;

   /*!
    * Zero-based text column number, counted from the starting character of a
    * text line, where a parsed node has been identified in an %XML document.
    * This member is -1 if text location information is not available.
    *
    * Note that the value stored in this field is actually a character index,
    * not necessarily a valid text column number. It is an actual column number
    * only if the corresponding line of text does not contain tabulator
    * characters (\#x9). If there are tabulators, there is usually no one-to-one
    * correspondence between characters and represented text columns.
    */
   int64 column = 0;

   /*!
    * Default constructor. Initializes the line and column members to -1,
    * signaling an undefined source code location.
    */
   XMLNodeLocation() = default;

   /*!
    * Constructs an %XMLNodeLocation object with the specified zero-based text
    * line and column numbers.
    */
   XMLNodeLocation( int line_, int column_ ) :
      line( line_ ), column( column_ )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLNodeLocation( const XMLNodeLocation& ) = default;

   /*!
    * Returns a string representation of this %XMLNodeLocation object in the
    * form " (line=n offset=m)", where n and m are, respectively, the one-based
    * line number and zero-based text character offset transported by this
    * object. This representation is suitable to be included directly in
    * warning or error messages generated by parsers.
    *
    * The line number token won't be generated if the text line number is
    * undefined (< 0) in this object. Similarly, the offset token won't be
    * generated if the column member is < 0. If no location information is
    * available, this member function returns an empty string.
    */
   String ToString() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLNode
 * \brief Abstract base class of all %XML document node classes
 *
 * %XML document nodes can be elements, text, CDATA sections, processing
 * instructions, comments, and unknown special elements. This class extends the
 * XMLComponent root base class to implement %XML document node classification
 * and serialization.
 *
 * \ingroup xml_parsing_and_generation
 */
class PCL_CLASS XMLNode : public XMLComponent
{
public:

   /*!
    * Represents the type of an %XML document node. Supported/valid node types
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
    * Virtual destructor.
    */
   virtual ~XMLNode()
   {
   }

   /*!
    * Returns true iff this is a child node of an existing %XML element.
    */
   bool IsChildNode() const
   {
      return m_type.IsFlagSet( XMLNodeType::ChildNode );
   }

   /*!
    * Returns the type of this %XML document node.
    */
   node_type NodeType() const
   {
      return static_cast<node_type>( XMLNodeTypes::flag_type( m_type & unsigned( ~XMLNodeType::ChildNode ) ) );
   }

   /*!
    * Returns true iff this node is an %XML element. If this member function
    * returns true, this node can be statically casted to XMLElement.
    */
   bool IsElement() const
   {
      return NodeType() == XMLNodeType::Element;
   }

   /*!
    * Returns true iff this node represents an %XML text block. If this member
    * function returns true, this node can be statically casted to XMLText.
    */
   bool IsText() const
   {
      return NodeType() == XMLNodeType::Text;
   }

   /*!
    * Returns true iff this node represents an %XML comment. If this member
    * function returns true, this node can be statically casted to XMLComment.
    */
   bool IsComment() const
   {
      return NodeType() == XMLNodeType::Comment;
   }

   /*!
    * Returns a reference to the (immutable) source code location of this node.
    */
   const XMLNodeLocation& Location() const
   {
      return m_location;
   }

   /*!
    * Serializes this document node as an %XML fragment encoded in UTF-8.
    *
    * \param text          Reference to an 8-bit string to which the UTF-8
    *             encoded serialization of this node must be appended.
    *
    * \param autoFormat    True if line break characters (\#x0A) and
    *             indentation strings must be used to improve readability of
    *             the generated %XML code. False if no superfluous white space
    *             should be generated.
    *
    * \param indentChar    A character used for indentation of generated text
    *             lines, when \a autoFormat is true. This parameter should be
    *             either a white space (' ' or \#x20) or a tabulator ('\\t' or
    *             \#x09) character.
    *
    * \param indentSize    Number of \a indentChar characters used for each
    *             indentation level, when \a autoFormat is true.
    *
    * \param level         Recursion level. A value greater than zero denotes
    *             that this function is being called from a parent %XML
    *             element. The recursion level determines the number of
    *             \a indentChar characters prepended to each text line
    *             for indentation, when \a autoFormat is true.
    */
   virtual void Serialize( IsoString& text, bool autoFormat, char indentChar, unsigned indentSize, unsigned level ) const = 0;

   /*!
    * Returns true iff a new line character (\#x0A) can be inserted before
    * serializing this node after the specified \a previous node.
    */
   virtual bool NLAfter( const XMLNode& previous ) const;

private:

   XMLNodeTypes    m_type;
   XMLNodeLocation m_location;

   friend class XMLDocument;
   friend class XMLElement;
};

/*!
 * \class pcl::XMLNodeList
 * \brief Dynamic list of %XML node objects
 *
 * %XMLNodeList is used as the internal implementation of element child node
 * lists and document node lists. In current PCL versions, %XMLNodeList is a
 * template instantiation of ReferenceArray<> for the XMLNode class.
 *
 * \ingroup xml_parsing_and_generation
 */
typedef ReferenceArray<XMLNode>  XMLNodeList;

// ----------------------------------------------------------------------------

/*!
 * \class XMLParseError
 * \brief %XML parsing error with automatic text location information
 * generation
 *
 * The %XMLParseError is useful to generate warning and error messages during
 * document parsing tasks, with automatic generation of text location
 * information (when available) and a normalized representation of error
 * messages.
 *
 * \ingroup xml_parsing_and_generation
 */
class PCL_CLASS XMLParseError : public Error
{
public:

   /*!
    * Constructs an %XMLParseError object with a reference to an XMLNode
    * instance.
    *
    * \param node          Reference to the XMLNode object that has caused the
    *                      exception.
    *
    * \param whileDoing    Identifies the parsing action that was taking place.
    *                      For example: "Parsing Metadata child Image element".
    *
    * \param whatHappened  Describes the error that has been detected. For
    *                      example: "Missing id attribute".
    *
    * This constructor inserts node location information, if available, and
    * joins the strings appropriately to build an error message.
    */
   XMLParseError( const XMLNode& node, const String& whileDoing, const String& whatHappened ) :
      Error( whileDoing + node.Location().ToString() + ": " + whatHappened )
   {
   }

   /*!
    * Constructs an %XMLParseError object with a reference to an
    * XMLNodeLocation instance.
    *
    * \param where         Reference to an XMLNodeLocation object to retrieve
    *                      text location information.
    *
    * \param whileDoing    Identifies the parsing action that was taking place.
    *                      For example: "Parsing Metadata child Image element".
    *
    * \param whatHappened  Describes the error that has been detected. For
    *                      example: "Missing id attribute".
    *
    * This constructor inserts node location information, if available, and
    * joins the strings appropriately to build an error message.
    */
   XMLParseError( const XMLNodeLocation& where, const String& whileDoing, const String& whatHappened ) :
      Error( whileDoing + where.ToString() + ": " + whatHappened )
   {
   }

   /*!
    * Copy constructor.
    */
   XMLParseError( const XMLParseError& ) = default;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLAttribute
 * \brief %XML element attribute
 *
 * The %XMLAttribute class represents an element attribute, as defined by the
 * Attribute construct:
 *
 * https://www.w3.org/TR/xml11/#NT-Attribute
 *
 * \ingroup xml_parsing_and_generation
 */
class PCL_CLASS XMLAttribute : public XMLComponent
{
public:

   /*!
    * Constructs an empty %XML attribute. An empty attribute is ignored for
    * inclusion in element attribute lists.
    */
   XMLAttribute() = default;

   /*!
    * Constructs a new %XMLAttribute object with the specified qualified
    * \a name and \a value.
    *
    * The specified \a name should be a valid %XML qualified name, as defined
    * by the W3C recommendation:
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
    * Sets a new value for this %XML element attribute.
    */
   void SetValue( const String& text )
   {
      m_value = text;
   }

   /*!
    * Returns an encoded version of the attribute value. All characters that
    * cannot legally occur in an %XML attribute value are replaced by their
    * corresponding entity references.
    */
   String EncodedValue() const
   {
      return XML::EncodedText( m_value, false/*apos*/ );
   }

   /*!
    * Equality operator.
    *
    * Two %XML element attributes are considered equal if their qualified names
    * are identical. Note that this restricts valid attribute comparisons to a
    * particular %XML document.
    */
   bool operator ==( const XMLAttribute& x ) const
   {
      return m_name == x.m_name;
   }

   /*!
    * Less-than relational operator.
    *
    * To compare %XML element attributes, only their qualified names are taken
    * into account. Note that this restricts valid attribute comparisons to a
    * particular %XML document.
    */
   bool operator <( const XMLAttribute& x ) const
   {
      return m_name < x.m_name;
   }

private:

   String m_name;
   String m_value;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLAttributeList
 * \brief Dynamic list of %XML element attributes
 *
 * %XMLAttributeList represents a sequence of %XML element attributes in a
 * start-tag, as defined in the W3C recommendation:
 *
 * https://www.w3.org/TR/xml11/#sec-starttags
 *
 * %XMLAttributeList is internally implemented as a dynamic array of
 * XMLAttribute objects.
 *
 * \ingroup xml_parsing_and_generation
 */
class PCL_CLASS XMLAttributeList
{
public:

   /*!
    * Represents the dynamic container class used internally to implement an
    * %XML element attribute list.
    */
   typedef Array<XMLAttribute>                  list_implementation;

   /*!
    * Represents a mutable %XML element attribute list iterator.
    */
   typedef list_implementation::iterator        iterator;

   /*!
    * Represents an immutable %XML element attribute list iterator.
    */
   typedef list_implementation::const_iterator  const_iterator;

   /*!
    * Constructs a new %XMLAttributeList object by parsing the specified
    * \a text string.
    *
    * The specified \a text must be a sequence of zero or more Attribute %XML
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
    * Default constructor. Constructs an empty %XML attribute list.
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
    * Causes this list to contain an %XML element attribute with the specified
    * qualified \a name and \a value.
    *
    * If an attribute with the same qualified \a name already exists in this
    * list, then its value will be changed. Otherwise, a new attribute will be
    * appended to this list.
    *
    * This member function ensures that no %XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * %XML specification:
    *
    * https://www.w3.org/TR/xml11/#sec-starttags
    * https://www.w3.org/TR/xml-names/#scoping-defaulting
    */
   void SetAttribute( const String& name, const String& value )
   {
      if ( !name.IsEmpty() )
      {
         iterator a = m_list.Search( name );
         if ( a == m_list.End() )
            m_list.Add( XMLAttribute( name, value ) );
         else
            a->SetValue( value );
      }
   }

   /*!
    * Causes this list to contain the specified %XML element \a attribute.
    *
    * See SetAttribute( const String&, const String& ) for more information.
    */
   void SetAttribute( const XMLAttribute& attribute )
   {
      if ( !attribute.Name().IsEmpty() )
      {
         iterator a = m_list.Search( attribute );
         if ( a == m_list.End() )
            m_list.Add( attribute );
         else
            *a = attribute;
      }
   }

   /*!
    * Insertion operator. Returns a reference to this object.
    *
    * This operator is equivalent to SetAttribute( const XMLAttribute& ).
    */
   XMLAttributeList& operator <<( const XMLAttribute& attribute )
   {
      SetAttribute( attribute );
      return *this;
   }

   /*!
    * Causes this list to contain the specified \a list of %XML element
    * attributes.
    *
    * For each attribute in the specified \a list, if an attribute with the
    * same qualified \a name already exists in this list, then its value will
    * be changed. Otherwise, a new attribute will be appended to this list.
    *
    * This member function ensures that no %XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * %XML specification:
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
    * Insertion operator. Returns a reference to this object.
    *
    * This operator is equivalent to SetAttributes( const XMLAttributeList& ).
    */
   XMLAttributeList& operator <<( const XMLAttributeList& list )
   {
      SetAttributes( list );
      return *this;
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
    * of %XML element attributes.
    *
    * The specified \a text must be a sequence of zero or more Attribute %XML
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
    * characters (\#x20).
    */
   void Parse( const String& text );

   /*!
    * Performs the %XML serialization of this element attribute list and
    * appends it to the specified \a text string, encoded in UTF-8.
    *
    * The generated serialization is a sequence of zero or more Attribute %XML
    * definitions pertaining to a start-tag, as described in the W3C
    * recommendation:
    *
    * https://www.w3.org/TR/xml11/#NT-STag
    */
   void Serialize( IsoString& text ) const;

private:

   list_implementation m_list;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLElement
 * \brief %XML element
 *
 * The %XMLElement class represents an %XML document element:
 *
 * https://www.w3.org/TR/xml11/#dt-element
 *
 * Elements are the main data holders in the logical design of %XML, following
 * a hierarchical tree structure.
 *
 * \ingroup xml_parsing_and_generation
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
    * A list of child %XML elements. Implemented as a template instantiation of
    * ReferenceArray<> for the XMLElement class.
    */
   typedef ReferenceArray<XMLElement>     child_element_list;

   /*!
    * Default constructor. Constructs an uninitialized %XMLElement structure.
    */
   XMLElement() :
      XMLNode( XMLNodeType::Element )
   {
   }

   /*!
    * Constructs an empty %XMLElement object with the specified qualified
    * \a name and \a attributes.
    */
   XMLElement( const String& name, const XMLAttributeList& attributes = XMLAttributeList() ) :
      XMLNode( XMLNodeType::Element ),
      m_name( name ),
      m_attributes( attributes )
   {
   }

   /*!
    * Constructs an empty %XMLElement object with the specified qualified
    * \a name and \a attributes, as a child node of the specified \a parent
    * element.
    */
   XMLElement( XMLElement& parent, const String& name, const XMLAttributeList& attributes = XMLAttributeList() ) :
      XMLNode( XMLNodeType::Element ),
      m_name( name ),
      m_attributes( attributes )
   {
      parent.AddChildNode( this );
   }

   /*!
    * Copy constructor. This constructor is disabled because %XMLElement
    * represents unique objects.
    */
   XMLElement( const XMLElement& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %XMLElement represents
    * unique objects.
    */
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
    * Returns true iff this is a root %XML element. A root %XML element has no
    * parent element.
    *
    * Note that this member function can return true in two different
    * situations: when the element has been generated during a document parsing
    * process (in which case this is an actual document root element), and if
    * this object has not been initialized yet (because it has been newly
    * constructed and still has not been associated with an %XML document).
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
    * Returns a copy of the list of %XML element attributes.
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
    * Causes this %XML element to contain an attribute with the specified
    * qualified \a name and \a value.
    *
    * If an attribute with the same qualified \a name already exists in this
    * element, then its value will be changed. Otherwise, a new attribute will
    * be created in this element.
    *
    * This member function ensures that no %XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * %XML specification:
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
    * Causes this %XML element to contain the specified \a attribute.
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
    * Insertion operator. Returns a reference to this object.
    *
    * This operator is equivalent to SetAttribute( const XMLAttribute& ).
    */
   XMLElement& operator <<( const XMLAttribute& attribute )
   {
      SetAttribute( attribute );
      return *this;
   }

   /*!
    * Causes this %XML element to contain the specified \a list of attributes.
    *
    * For each attribute in the specified \a list, if an attribute with the
    * same qualified \a name already exists in this element, then its value
    * will be changed. Otherwise, a new attribute will be created in this
    * element.
    *
    * This member function ensures that no %XML element can have two or more
    * attributes with the same qualified name. This constraint is part of the
    * %XML specification:
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
    * Insertion operator. Returns a reference to this object.
    *
    * This operator is equivalent to SetAttributes( const XMLAttributeList& ).
    */
   XMLElement& operator <<( const XMLAttributeList& list )
   {
      SetAttributes( list );
      return *this;
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
    * of attributes in this %XML element. The previous list of attributes, if
    * any, will be replaced by the newly generated list.
    *
    * The specified \a text must be a sequence of zero or more Attribute %XML
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
    * Performs the %XML serialization of the attribute list in this element and
    * appends it to the specified \a text string, encoded in UTF-8.
    *
    * The generated serialization is a sequence of zero or more Attribute %XML
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
    * Returns the number of child nodes in this %XML element.
    */
   int ChildCount() const
   {
      return int( m_childNodes.Length() );
   }

   /*!
    * Returns true iff this is an empty %XML element. An empty element has no
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
    * child nodes of this %XML element.
    */
   const_iterator Begin() const
   {
      return m_childNodes.Begin();
   }

   /*!
    * Returns an immutable iterator located at the end of the list of child
    * nodes of this %XML element.
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
    * Returns true iff this element contains one or more child %XML elements.
    */
   bool HasElements() const
   {
      return m_childTypes.IsFlagSet( XMLNodeType::Element );
   }

   /*!
    * Returns true iff this element contains one or more child text blocks.
    */
   bool HasText() const
   {
      return m_childTypes.IsFlagSet( XMLNodeType::Text );
   }

   /*!
    * Returns true iff this element contains one or more child CDATA sections.
    */
   bool HasCDATA() const
   {
      return m_childTypes.IsFlagSet( XMLNodeType::CDATA );
   }

   /*!
    * Returns true iff this element contains one or more child processing
    * instructions.
    */
   bool HasProcessingInstructions() const
   {
      return m_childTypes.IsFlagSet( XMLNodeType::ProcessingInstructions );
   }

   /*!
    * Returns true iff this element contains one or more child comment
    * sections.
    */
   bool HasComments() const
   {
      return m_childTypes.IsFlagSet( XMLNodeType::Comment );
   }

   /*!
    * Returns the text contents of this element, or an empty string if this
    * element has no text child nodes.
    *
    * If this element has two or more text child nodes, the returned value is
    * the concatenation of all child text nodes.
    */
   String Text() const;

   /*!
    * \internal
    */
   void GetChildElements( child_element_list& list, bool recursive ) const
   {
      for ( const XMLNode& node : m_childNodes )
         if ( node.IsElement() )
         {
            const XMLElement& element = static_cast<const XMLElement&>( node );
            list << &element;
            if ( recursive )
               element.GetChildElements( list, recursive );
         }
   }

   /*!
    * Returns a list with all child elements of this element.
    *
    * if \a recursive is \c true, this member function performs a recursive
    * search across the entire tree structure rooted at this element. Otherwise
    * only the direct descendant elements will be returned.
    */
   child_element_list ChildElements( bool recursive = false ) const
   {
      child_element_list list;
      GetChildElements( list, recursive );
      return list;
   }

   /*!
    * \internal
    */
   void GetChildElementsByName( child_element_list& list, const String& name, bool recursive ) const
   {
      for ( const XMLNode& node : m_childNodes )
         if ( node.IsElement() )
         {
            const XMLElement& element = static_cast<const XMLElement&>( node );
            if ( element.Name() == name )
            {
               list << &element;
               if ( recursive )
                  element.GetChildElementsByName( list, name, recursive );
            }
         }
   }

   /*!
    * Returns a list with all child elements of this element with the specified
    * \a name.
    *
    * if \a recursive is \c true, this member function performs a recursive
    * search across the entire tree structure rooted at this element. Otherwise
    * only the direct descendant elements will be returned.
    */
   child_element_list ChildElementsByName( const String& name, bool recursive = false ) const
   {
      child_element_list list;
      GetChildElementsByName( list, name, recursive );
      return list;
   }

   /*!
    * \internal
    */
   void GetChildNodesByType( XMLNodeList& list, XMLNodeTypes types, bool recursive ) const
   {
      for ( const XMLNode& node : m_childNodes )
         if ( types.IsFlagSet( node.NodeType() ) )
         {
            list << &node;
            if ( recursive )
               if ( node.IsElement() )
                  static_cast<const XMLElement&>( node ).GetChildNodesByType( list, types, recursive );
         }
   }

   /*!
    * Returns a list with all child nodes of this element of the specified
    * \a types. The \a types argument can be an ORed combination of XMLNodeType
    * enumerated mask values.
    *
    * if \a recursive is \c true, this member function performs a recursive
    * search across the entire tree structure rooted at this element. Otherwise
    * only the direct descendant nodes will be returned.
    */
   XMLNodeList ChildNodesByType( XMLNodeTypes types, bool recursive = false ) const
   {
      XMLNodeList list;
      GetChildNodesByType( list, types, recursive );
      return list;
   }

   /*!
    * \internal
    */
   template <class UP>
   void GetChildNodesThat( XMLNodeList& list, UP u, bool recursive ) const
   {
      for ( const XMLNode& node : m_childNodes )
         if ( u( node ) )
         {
            list << &node;
            if ( recursive )
               if ( node.IsElement() )
                  static_cast<const XMLElement&>( node ).GetChildNodesThat( list, u, recursive );
         }
   }

   /*!
    * Returns a list with all child nodes of this element that satisfy the
    * specified unary predicate \a u.
    *
    * For each child node n in this element, n will be included in the returned
    * list iff u( n ) returns true.
    *
    * if \a recursive is \c true, this member function performs a recursive
    * search across the entire tree structure rooted at this element. Otherwise
    * only the direct descendant nodes will be returned.
    */
   template <class UP>
   XMLNodeList ChildNodesThat( UP u, bool recursive = false ) const
   {
      XMLNodeList list;
      GetChildNodesThat( list, u, recursive );
      return list;
   }

   /*!
    * Appends a new child \a node to this %XML element.
    *
    * The specified \a node will be owned by this element, which will destroy
    * it automatically (and recursively) upon destruction.
    */
   void AddChildNode( XMLNode* node )
   {
      m_childNodes << node;
      node->m_parent = this;
      node->m_type.SetFlag( XMLNodeType::ChildNode );
      m_childTypes.SetFlag( node->NodeType() );
   }

   /*!
    * Insertion operator. Returns a reference to this object.
    *
    * This operator is equivalent to AddChildNode( node ).
    */
   XMLElement& operator <<( XMLNode* node )
   {
      AddChildNode( node );
      return *this;
   }

   /*!
    * \internal
    * Appends a new child \a node to this %XML element.
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
    * Recursively destroys all existing child nodes in this %XML element,
    * yielding an empty element.
    */
   void DestroyChildNodes()
   {
      m_childNodes.Destroy();
      m_childTypes = XMLNodeType::Undefined;
   }

   /*!
    * Recursively serializes this %XML element and its contents. Appends the
    * generated %XML source code to the specified 8-bit \a text string, encoded
    * in UTF-8.
    *
    * See XMLNode::Serialize() for information on function parameters.
    */
   virtual void Serialize( IsoString& text, bool autoFormat, char indentChar, unsigned indentSize, unsigned level ) const;

private:

   String           m_name;
   XMLAttributeList m_attributes;
   XMLNodeList      m_childNodes;
   XMLNodeTypes     m_childTypes = XMLNodeType::Undefined;
};

// ----------------------------------------------------------------------------

/*!
 * \class pcl::XMLElementList
 * \brief Dynamic list of %XML elements
 *
 * %XMLElementList is a template instantiation of ReferenceArray<> for the
 * XMLElement class. It is used to transport ordered sequences of child element
 * nodes. See for example XMLElement::ChildElements().
 *
 * \ingroup xml_parsing_and_generation
 */
typedef XMLElement::child_element_list XMLElementList;

// ----------------------------------------------------------------------------

/*!
 * \class XMLText
 * \brief %XML text block
 *
 * This %XMLText class represents a text entity in an %XML document:
 *
 * https://www.w3.org/TR/xml11/#dt-text
 *
 * \ingroup xml_parsing_and_generation
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
    *                space characters (\#x20). If true, the specified \a text
    *                string will be stored intact.
    *
    * Besides text contents transformation, space preservation also has an
    * impact in the way text blocks are serialized as %XML: New line characters
    * (\#x0A) are never used to separate text blocks from their parent or
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
    * Returns a reference to the (immutable) text string contained by this %XML
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
    * legally occur in an %XML text block are replaced by their corresponding
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
    *                   with single white space characters (\#x20).
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
    * Serializes this %XML text block with UTF-8 encoding.
    *
    * See XMLNode::Serialize() for information on function parameters. See also
    * the class constructor for information on space preservation options in
    * %XML text blocks.
    */
   virtual void Serialize( IsoString& text, bool autoFormat, char indentChar, unsigned indentSize, unsigned level ) const;

   /*!
    * Returns true iff a new line character (\#x0A) can be inserted before
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

// ----------------------------------------------------------------------------

/*!
 * \class XMLCDATA
 * \brief %XML CDATA section
 *
 * The %XMLCDATA class represents a CDATA section in an %XML document:
 *
 * https://www.w3.org/TR/xml11/#sec-cdata-sect
 *
 * \ingroup xml_parsing_and_generation
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
    * Serializes this %XML CDATA section with UTF-8 encoding.
    *
    * See XMLNode::Serialize() for information on function parameters.
    */
   virtual void Serialize( IsoString& text, bool autoFormat, char indentChar, unsigned indentSize, unsigned level ) const;

private:

   String m_cdata;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLProcessingInstructions
 * \brief %XML processing instructions
 *
 * The %XMLProcessingInstructions class represents a processing instructions
 * (PI) tag in an %XML document:
 *
 * https://www.w3.org/TR/xml11/#sec-pi
 *
 * \ingroup xml_parsing_and_generation
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
    * Serializes this %XML PI section with UTF-8 encoding.
    *
    * See XMLNode::Serialize() for information on function parameters.
    */
   virtual void Serialize( IsoString& text, bool autoFormat, char indentChar, unsigned indentSize, unsigned level ) const;

private:

   String m_target;
   String m_instructions;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLComment
 * \brief %XML comment section
 *
 * The %XMLComment class represents a comment in an %XML document:
 *
 * https://www.w3.org/TR/xml11/#sec-comments
 *
 * \ingroup xml_parsing_and_generation
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
    * Serializes this %XML comment section with UTF-8 encoding.
    *
    * See XMLNode::Serialize() for information on function parameters.
    */
   virtual void Serialize( IsoString& text, bool autoFormat, char indentChar, unsigned indentSize, unsigned level ) const;

private:

   String m_comment;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLUnknownElement
 * \brief Unsupported or invalid %XML element
 *
 * %XMLUnknownElement represents an invalid or unrecognized %XML element
 * retrieved while parsing an %XML document. In the current PCL implementation,
 * an %XMLUnknownElement object is generated if the parser finds an element
 * whose start-tag begins with the "<!" token and is neither a comment section
 * nor a DOCTYPE declaration.
 *
 * \ingroup xml_parsing_and_generation
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
    * Serializes this %XML unknown element with UTF-8 encoding.
    *
    * See XMLNode::Serialize() for information on function parameters.
    */
   virtual void Serialize( IsoString& text, bool autoFormat, char indentChar, unsigned indentSize, unsigned level ) const;

private:

   String m_name;
   String m_parameters;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLDeclaration
 * \brief %XML declaration
 *
 * %XMLDeclaration represents an %XML declaration in an %XML document prolog:
 *
 * https://www.w3.org/TR/xml11/#sec-prolog-dtd
 *
 * \ingroup xml_parsing_and_generation
 */
class PCL_CLASS XMLDeclaration : public XMLComponent
{
public:

   /*!
    * Constructs a new %XMLDeclaration object with the specified \a version,
    * optional \a encoding and \a standalone document specification.
    */
   XMLDeclaration( const String& version = String(), const String& encoding = String(), bool standalone = false ) :
      XMLComponent(),
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
    * Returns a reference to the (immutable) %XML version string.
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
    * Returns true iff this %XML declaration specifies a standalone document.
    */
   bool IsStandaloneDocument() const
   {
      return m_standalone;
   }

   /*!
    * Returns true iff this %XML declaration has been defined. This function
    * can be used to check if a parsed %XML document includes an %XML
    * declaration.
    */
   bool IsDefined() const
   {
      return !m_version.IsEmpty();
   }

   /*!
    * Serializes this %XML declaration.
    *
    * The generated serialization will be appended to the specified 8-bit
    * \a text string. If no version string has been defined for this object, a
    * 'version="1.0"' attribute will be generated. Similarly, if no encoding
    * string has been defined, an 'encoding="UTF-8"' attribute will be
    * generated.
    */
   void Serialize( IsoString& text ) const;

private:

   String m_version;
   String m_encoding;
   bool   m_standalone : 1;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLDocTypeDeclaration
 * \brief %XML DOCTYPE declaration
 *
 * %XMLDocTypeDeclaration represents a document type declaration in an %XML
 * document prolog:
 *
 * https://www.w3.org/TR/xml11/#dt-doctype
 *
 * \ingroup xml_parsing_and_generation
 */
class PCL_CLASS XMLDocTypeDeclaration : public XMLComponent
{
public:

   /*!
    * Constructs a new %XMLDocTypeDeclaration object with the specified
    * document type \a name and type \a definition.
    */
   XMLDocTypeDeclaration( const String& name = String(), const String& definition = String() ) :
      XMLComponent(),
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
    * function can be used to check if a parsed %XML document includes an
    * DOCTYPE declaration.
    */
   bool IsDefined() const
   {
      return !m_name.IsEmpty();
   }

   /*!
    * Serializes this DOCTYPE declaration with UTF-8 encoding.
    *
    * The generated serialization will be appended to the specified 8-bit
    * \a text string, encoded in UTF-8. This function won't generate any
    * characters if no document type name has been defined for this object.
    */
   void Serialize( IsoString& text ) const;

private:

   String m_name;
   String m_definition;
};

// ----------------------------------------------------------------------------

/*!
 * \class XMLElementFilter
 * \brief A functional class for filtering %XML elements
 *
 * Element filters can be used with XMLDocument objects to reject elements
 * selectively while parsing an %XML document.
 *
 * When an element filter has been defined, %XMLDocument calls its
 * reimplemented operator()( const XMLElement*, const String& ) member function
 * when an element's start-tag is found, passing its parent element and name to
 * the function. If the function returns false, the element is rejected,
 * including all of its child nodes, and the parsing process continues after
 * the corresponding end-tag.
 *
 * If the first function call described above returns true, %XMLDocument calls
 * operator()( const XMLElement*, const String&, const XMLAttributeList& ) with
 * the parent, name and attributes of the element. Again, if the function
 * returns false the element is skipped completely until its end-tag is found.
 * If the function returns true, the element is accepted, added to the DOM
 * being generated, and parsed.
 *
 * Element filters can be useful to accelerate %XML document parsing and reduce
 * its memory consumption considerably, when only a subset of possible elements
 * is required.
 *
 * \ingroup xml_parsing_and_generation
 */
struct XMLElementFilter
{
   /*!
    * Virtual destructor.
    */
   virtual ~XMLElementFilter()
   {
   }

   /*!
    * Returns true if an %XML element with the specified \a name and \a parent
    * element is acceptable; false if the element must be ignored.
    */
   virtual bool operator()( const XMLElement* parent, const String& name ) const = 0;

   /*!
    * Returns true if an %XML element with the specified \a name, \a attributes
    * and \a parent element is acceptable; false if the element must be
    * ignored. This function is only called after a previous call to
    * operator()( const XMLElement*, const String& ) has returned true for the
    * same element.
    *
    * The default implementation returns true, which facilitates
    * implementations where elements are only filtered by their names.
    */
   virtual bool operator()( const XMLElement* parent, const String& name, const XMLAttributeList& attributes ) const
   {
      return true;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::XMLParserOption
 * \brief     %XML document parsing options
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>XMLParserOption::IgnoreComments</td>           <td>Do not add comment nodes to the DOM.</td></tr>
 * <tr><td>XMLParserOption::IgnoreUnknownElements</td>    <td>Do not add unknown/invalid elements to the DOM.</td></tr>
 * <tr><td>XMLParserOption::IgnoreStrayCharacters</td>    <td>Be tolerant of non-space characters outside markup.</td></tr>
 * <tr><td>XMLParserOption::NormalizeTextSpaces</td>      <td>Trim and collapse spaces in all child text nodes.</td></tr>
 * </table>
 *
 * \ingroup xml_parsing_and_generation
 */
namespace XMLParserOption
{
   enum mask_type
   {
      IgnoreComments        = 0x00000001,
      IgnoreUnknownElements = 0x00000002,
      IgnoreStrayCharacters = 0x00000004,
      NormalizeTextSpaces   = 0x00000008
   };
}

/*!
 * \class pcl::XMLParserOptions
 * \brief A collection of %XML document parsing options
 * \ingroup xml_parsing_and_generation
 */
typedef Flags<XMLParserOption::mask_type>  XMLParserOptions;

// ----------------------------------------------------------------------------

/*!
 * \class XMLDocument
 * \brief %XML document parsing and generation
 *
 * %XMLDocument implements parsing and generation of well-formed %XML
 * documents.
 *
 * The Parse() member function reads and interprets a Unicode text string to
 * generate a read-only document object model (DOM) that represents the data
 * entities defined by a well-formed %XML document. The DOM can be inspected
 * with several member functions of the %XMLDocument class. All %XML nodes and
 * elements in a document can be visited recursively with specialized accessor
 * functions and iterators. See the Begin() and End() functions (and their
 * STL-compatible equivalents, begin() and end()), as well as XML(), DocType(),
 * RootElement(), and operator []( int ), among others.
 *
 * For generation of %XML documents, the Serialize() member function builds a
 * new document as a Unicode string encoded in UTF-8. The document's root node
 * and several nodes and critical components must be defined before document
 * generation - see the SetXML(), SetDocType(), AddNode() and SetRootElement()
 * member functions.
 *
 * For general information on %XML, the authoritative sources are the W3C
 * recommendations:
 *
 * https://www.w3.org/TR/xml/
 * https://www.w3.org/TR/xml11/
 * https://www.w3.org/TR/xml-names/
 *
 * The following example shows how an existing document can be parsed as a new
 * %XMLDocument object, and then a new %XML document can be generated and
 * written to a disk file, all in just three source code lines:
 *
 * \code
 * XMLDocument xml;
 * xml.Parse( File::ReadTextFile( "/path/to/file.xml" ).UTF8ToUTF16() );
 * File::WriteTextFile( "/tmp/test.xml", xml.Serialize() );
 * \endcode
 *
 * In this case the new document is generated without superfluous space
 * characters. To enable automatic indentation of text lines, see the
 * EnableAutoFormatting(), SetIndentSize() and EnableIndentTabs() member
 * functions.
 *
 * The following example:
 *
 * \code
 * XMLElement* e1 = new XMLElement( "Foo", XMLAttributeList() << XMLAttribute( "version", "1.0" ) );
 *
 * XMLElement* e2 = new XMLElement( "Bar" );
 * *e2 << new XMLElement( "bar_child_1" )
 *     << new XMLElement( "bar_child_2" );
 *
 * XMLElement* e3 = new XMLElement( "FooBar" );
 * *e3 << new XMLText( "This is FooBar." );
 *
 * *e1 << e2 << e3;
 *
 * XMLDocument xml;
 * xml.SetXML( "1.0" );
 * xml.SetRootElement( e1 );
 * xml.EnableAutoFormatting();
 * xml.SerializeToFile( "/tmp/foobar.xml" );
 * \endcode
 *
 * generates this %XML file in /tmp/foobar.xml:
 *
 * \code
 * <?xml version="1.0" encoding="UTF-8"?>
 * <Foo version="1.0">
 *    <Bar>
 *       <bar_child_1/>
 *       <bar_child_2/>
 *    </Bar>
 *    <FooBar>This is FooBar.</FooBar>
 * </Foo>
 * \endcode
 *
 * \ingroup xml_parsing_and_generation
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
    * Represents an option to control the %XML parser behavior. Valid options
    * are defined in the XMLParserOption namespace.
    */
   typedef XMLParserOption::mask_type     parser_option;

   /*!
    * Default constructor. Constructs an empty %XML document.
    *
    * For serialization of XML documents, this constructor defines the
    * following default settings:
    *
    * \li Auto-formatting disabled.
    * \li Use space characters (\#x20) for indentation.
    * \li Indentation size = 3 spaces.
    */
   XMLDocument()
   {
      m_autoFormatting = false;
      m_indentTabs = false;
      m_indentSize = 3;
   }

   /*!
    * Virtual destructor. Recursively destroys all %XML elements, declarations
    * and auxiliary data associated with this object.
    */
   virtual ~XMLDocument()
   {
      m_nodes.Destroy();
      m_root = nullptr;
      RemoveElementFilter();
   }

   /*!
    * Copy constructor. This constructor is disabled because %XMLDocument
    * represents unique objects.
    */
   XMLDocument( const XMLDocument& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %XMLDocument
    * represents unique objects.
    */
   XMLDocument& operator =( const XMLDocument& ) = delete;

   /*!
    * Returns a reference to the (immutable) %XML declaration object associated
    * with this document.
    */
   const XMLDeclaration& XML() const
   {
      return m_xml;
   }

   /*!
    * Defines an %XML declaration in this %XML document.
    */
   void SetXML( const XMLDeclaration& xml )
   {
      m_xml = xml;
   }

   /*!
    * Defines an %XML declaration in this %XML document with the specified
    * \a version, \a encoding and \a standalone attributes.
    */
   void SetXML( const String& version = "1.0", const String& encoding = "UTF-8", bool standalone = false )
   {
      SetXML( XMLDeclaration( version, encoding, standalone ) );
   }

   /*!
    * Returns a reference to the (immutable) %XML document type declaration
    * object associated with this document.
    */
   const XMLDocTypeDeclaration& DocType() const
   {
      return m_docType;
   }

   /*!
    * Associates a new %XML document type declaration object with this %XML
    * document.
    */
   void SetDocType( const XMLDocTypeDeclaration& docType )
   {
      m_docType = docType;
   }

   /*!
    * Returns a pointer to the (immutable) root element of this %XML document.
    * If there is no root element, for example when this is an uninitialized
    * %XMLDocument instance, this function returns \c nullptr.
    *
    * \sa ReleaseRootElement(), SetRootElement()
    */
   const XMLElement* RootElement() const
   {
      return m_root;
   }

   /*!
    * Releases the root element of this %XML document.
    *
    * This function returns the root element and causes this object to forget
    * it. The caller will be responsible for destroying and deallocating the
    * returned XMLElement instance as appropriate. This function performs an
    * implicit call to Clear(), so the document will be empty after calling it.
    *
    * If there is no root element, for example when this is an uninitialized
    * %XMLDocument instance, this function returns \c nullptr.
    *
    * \sa RootElement(), SetRootElement()
    */
   XMLElement* ReleaseRootElement()
   {
      XMLElement* root = m_root;
      m_nodes.RemovePointer( m_root );
      Clear();
      return root;
   }

   /*!
    * Returns the number of nodes in this %XML document, or zero if this is an
    * empty or uninitialized %XMLDocument object.
    */
   int NodeCount() const
   {
      return int( m_nodes.Length() );
   }

   /*!
    * Returns true iff this is an empty %XML document. An empty document has no
    * %XML nodes.
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
    * nodes of this %XML document.
    */
   const_iterator Begin() const
   {
      return m_nodes.Begin();
   }

   /*!
    * Returns an immutable iterator located at the end of the list of
    * nodes of this %XML document.
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
    * Appends a new top-level %XML node to this document.
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
    * Insertion operator. Returns a reference to this %XMLDocument object.
    *
    * This operator is equivalent to AddNode( XMLNode* ).
    */
   XMLDocument& operator <<( XMLNode* node )
   {
      AddNode( node );
      return *this;
   }

   /*!
    * Sets the root element of this %XML document.
    *
    * If the specified \a element already belongs to an %XMLDocument object, if
    * a null pointer is specified, or if a root node has already been defined
    * for this document, this member function will throw an Error exception.
    *
    * The specified \a element will be appended to the current list of document
    * nodes. The \a element will be owned by this document object, which will
    * destroy and deallocate it automatically when appropriate.
    *
    * \sa RootElement(), ReleaseRootElement()
    */
   void SetRootElement( XMLElement* element );

   /*!
    * Destroys and deallocates all nodes and elements in this %XML document
    * object, and initializes all internal structures to a default state,
    * yielding an uninitialized object.
    *
    * If there is an element filter or a set of parser options defined for this
    * object, they are preserved by this function. See RemoveElementFilter() to
    * remove a filter set by a previous call to SetElementFilter(). See also
    * ClearParserOptions() to reset parser options set by previous calls to
    * SetParserOption().
    */
   void Clear();

   /*!
    * Sets a new element filter for this object. The specified object will be
    * owned by this %XMLDocument instance, which will destroy and deallocate it
    * when appropriate.
    *
    * See XMLElementFilter for a complete description of the element filtering
    * functionality. See RemoveElementFilter() to remove the element filter
    * set by this function.
    */
   void SetElementFilter( XMLElementFilter* filter )
   {
      delete m_filter, m_filter = filter;
   }

   /*!
    * Removes an element filter set by a previous call to SetElementFilter().
    * If no filter has been defined for this object, this function has no
    * effect.
    */
   void RemoveElementFilter()
   {
      SetElementFilter( nullptr );
   }

   /*!
    * Enables or disables an %XML document parser option for this object. Valid
    * options are defined in the XMLParserOption namespace. See
    * ClearParserOptions() to reset all parser options to a default state.
    */
   void SetParserOption( parser_option option, bool on = true )
   {
      m_parserOptions.SetFlag( option, on );
   }

   /*!
    * Sets the specified parser \a options. Valid options are defined in the
    * XMLParserOption namespace. See ClearParserOptions() to reset all parser
    * options to a default state.
    */
   void SetParserOptions( XMLParserOptions options )
   {
      m_parserOptions = options;
   }

   /*!
    * Resets all parser options defined for this object by a previous call to
    * SetParserOption() or SetParserOptions().
    */
   void ClearParserOptions()
   {
      m_parserOptions.Clear();
   }

   /*!
    * %XML document parser. Reads and interprets the specified Unicode \a text
    * string, which must be encoded in UTF-16, as a well-formed %XML document.
    *
    * This member function generates a document object model (DOM) to represent
    * the data entities defined by the source %XML document. The DOM can then
    * be inspected with several member functions of the %XMLDocument class. All
    * %XML nodes and elements can be visited recursively with specialized
    * iterators. See the Begin() and End() functions (and their STL-compatible
    * equivalents, begin() and end()), as well as XML(), DocType(),
    * RootElement() and operator []( int ), among others.
    */
   void Parse( const String& text );

   /*!
    * Returns true iff the auto-formatting feature is enabled for %XML
    * serialization with this %XMLDocument object.
    *
    * When auto-formatting is enabled, ignorable line breaks (\#x0A) and white
    * space characters (either spaces (\#x20) or tabulators (\#x09)) are used
    * to separate %XML nodes and to indent text lines, respectively, improving
    * readability of generated %XML code. When auto-formatting is disabled, no
    * superfluous white space characters are generated. The only exception is
    * XMLText child nodes with space preservation enabled, which always ignore
    * all indentation and formatting settings in order to reproduce their text
    * contents without modification.
    *
    * The auto-formatting feature is always disabled by default for newly
    * constructed %XMLDocument objects. This is because the main purpose and
    * utility of %XMLDocument is parsing and generation of %XML documents
    * intended for automated data management, without direct user intervention.
    * Auto-formatting is only useful for human readability of %XML source code.
    */
   bool IsAutoFormatting() const
   {
      return m_autoFormatting;
   }

   /*!
    * Enables the auto-formatting feature for generation of %XML code. See
    * IsAutoFormatting() for more information.
    */
   void EnableAutoFormatting( bool enable = true )
   {
      m_autoFormatting = enable;
   }

   /*!
    * Disables the auto-formatting feature for generation of %XML code. See
    * IsAutoFormatting() for more information.
    */
   void DisableAutoFormatting( bool disable = true )
   {
      EnableAutoFormatting( !disable );
   }

   /*!
    * Returns the number of space characters (\#x20) used for each indentation
    * level of text lines, when the auto-formatting feature is enabled and
    * space characters are used for indentation.
    *
    * When tabulator characters (\#x09) are used for indentation, this setting
    * is ignored and a single tabulator is always used for each indentation
    * level. See IsAutoFormatting() and SetIndentSize() for more information.
    */
   int IndentSize() const
   {
      return int( m_indentSize );
   }

   /*!
    * Sets the number of indentation space characters.
    *
    * \param indentSize    Number of space characters (\#x20) used for a level
    *                      of indentation of text lines in generated %XML code,
    *                      when the auto-formatting feature is enabled and
    *                      space characters are used for indentation. The valid
    *                      range of values is from zero (for no indentation) to
    *                      8 characters.
    *
    * When the indentation size is zero and auto-formatting is enabled, each
    * document node is generated in a separate line without any indentation.
    * XMLText child nodes with space preservation enabled will always ignore
    * all indentation and formatting settings, in order to reproduce their text
    * contents without modification.
    *
    * When tabulator characters (\#x09) are used for indentation, this setting
    * is ignored and a single tabulator character is always used for each
    * indentation level.
    *
    * The default indentation size is 3 for newly constructed %XMLDocument
    * objects.
    */
   void SetIndentSize( int indentSize )
   {
      m_indentSize = unsigned( Range( indentSize, 0, 8 ) );
   }

   /*!
    * Returns true if tabulator characters (\#x09) are used for indentation of
    * text lines, when the auto-formatting feature is enabled. Returns false if
    * space characters (\#x20) are used for indentation.
    *
    * By default, text indentation is always performed using space characters
    * by newly constructed %XMLDocument objects.
    */
   bool IsIndentTabs() const
   {
      return m_indentTabs;
   }

   /*!
    * Enables the use of tabulator characters (\#x09) for indentation. See
    * IsIndentTabs() for more information.
    */
   void EnableIndentTabs( bool enable = true )
   {
      m_indentTabs = enable;
   }

   /*!
    * Disables the use of tabulator characters (\#x09) for indentation. See
    * IsIndentTabs() for more information.
    */
   void DisableIndentTabs( bool disable = true )
   {
      EnableIndentTabs( !disable );
   }

   /*!
    * Serializes this %XML document. Returns the generated serialization as a
    * Unicode string encoded in UTF-8.
    *
    * To serialize a well-formed %XML document, this object must be initialized
    * first by defining a root element (see SetRootElement()) and other
    * document nodes, as necessary (see SetXML(), SetDocType(), and AddNode()).
    *
    * For formatting and indentation settings, see IsAutoFormatting(),
    * IndentSize() and IsIndentTabs().
    */
   IsoString Serialize() const;

   /*!
    * Serializes this %XML document and writes the result to a file at the
    * specified \a path with UTF-8 encoding.
    *
    * See Serialize() for more information.
    *
    * \warning If a file already exists at the specified path, its previous
    * contents will be lost after calling this function.
    */
   void SerializeToFile( const String& path ) const;

private:

   XMLDeclaration        m_xml;
   XMLDocTypeDeclaration m_docType;
   XMLNodeList           m_nodes;
   XMLElement*           m_root = nullptr;
   XMLElementFilter*     m_filter = nullptr;
   XMLParserOptions      m_parserOptions;
   XMLNodeLocation       m_location;
   bool                  m_autoFormatting : 1;
   bool                  m_indentTabs     : 1;
   unsigned              m_indentSize     : 4;
};

// ----------------------------------------------------------------------------




} // pcl

#endif   // __PCL_XML_h

// ----------------------------------------------------------------------------
// EOF pcl/XML.h - Released 2017-06-17T10:55:43Z
