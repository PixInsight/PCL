//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/Arguments.h - Released 2017-07-18T16:13:52Z
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

#ifndef __PCL_Arguments_h
#define __PCL_Arguments_h

/// \file pcl/Arguments.h

#include <pcl/Defs.h>

#include <pcl/Flags.h>
#include <pcl/String.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup argument_parsing Argument Parsing Routines and Utilities
 */

// ----------------------------------------------------------------------------

/*!
 * \class Argument
 * \brief A command-line argument
 *
 * %Argument implements a command-line argument that can be used with the
 * ParseArguments() function.
 *
 * Command-line arguments can be \e parametric and \e non-parametric
 * arguments (also known as \e item \e arguments).
 *
 * <b>1. Parametric Arguments</b>
 *
 * Parametric arguments specify \e modifiers or parameter values of different
 * types to their receiving processes. Syntactically, parametric arguments are
 * formalized as:
 *
 * <tt>-&lt;arg_id&gt;[&lt;arg_value&gt;]</tt>
 *
 * where the starting hypen character '-' is the <em>standard parameter
 * prefix.</em> &lt;arg_id&gt; is the <em>argument identifier</em> and
 * &lt;arg_value&gt; is an optional <em>argument value</em> specification. For
 * example, in the following parametric arguments:
 *
 * <tt>-n=17 --use-all-processors</tt>
 *
 * the argument identifiers are 'n' and '-use-all-processors', respectively,
 * and '=17' is an argument value specification.
 *
 * We consider four types of parametric arguments: \e literal, \e switch,
 * \e numeric and \e string arguments.
 *
 * <b>1.1 Literal Arguments</b>
 *
 * Literal arguments are parametric arguments that don't have a particular
 * value. They are modifiers that act as functions of their existence. For
 * example, in the following arguments:
 *
 * <tt>-c -x=10 -b="none" --verbose</tt>
 *
 * '-c' and '--verbose' are literal arguments.
 *
 * <b>1.2 Switch Arguments</b>
 *
 * Switch arguments are parametric arguments that can only have a boolean
 * \e enabled/disabled state. Switch arguments are of the form:
 *
 * <tt>-&lt;arg_id&gt;+|-</tt>
 *
 * where the + suffix means 'enabled' and the - suffix means 'disabled'.
 * For example, here are some switch arguments:
 *
 * <tt>-u+ -color- --mySwitchArgument+</tt>
 *
 * It is customary to allow the use of switch arguments also as literal
 * arguments. When switch arguments are used as literal arguments, a default
 * 'enabled' or 'disabled' state is assumed, depending on the functionality and
 * purpose of each argument. When switch arguments can also be literal, they
 * are formalized as:
 *
 * <tt>-&lt;arg_id&gt;[+|-]</tt>
 *
 * indicating that the + and - suffixes are optional.
 *
 * <b>1.3 Numeric Arguments</b>
 *
 * A numeric argument has an integer or floating-point numeric value. Numeric
 * arguments are formalized as:
 *
 * <tt>-&lt;arg_id&gt;=&lt;numeric_constant&gt;</tt>
 *
 * where &lt;numeric_constant&gt; is a valid representation of an integer or
 * floating-point real constant, following the applicable syntax rules of the C
 * language. Integer constants can have the '0x' standard prefix to indicate
 * hexadecimal values.
 *
 * <b>1.4 %String Arguments</b>
 *
 * A string argument has a literal value represented as a string of ISO 8859-1
 * characters. String arguments are formalized as:
 *
 * <tt>-&lt;arg_id&gt;=&lt;string_constant&gt;</tt>
 *
 * where &lt;string_constant&gt; is any sequence of ISO 8859-1 characters,
 * optionally enclosed by double quotes. For example:
 *
 * <tt>-s=foo -s1="bar"</tt>
 *
 * are two string arguments with 'foo' and 'bar' values, respectively. When the
 * &lt;string_constant&gt; string includes white spaces, the enclosing quotes are
 * not optional, as in:
 *
 * <tt>-aStringArg="this is a string literal"</tt>
 *
 * or otherwise the above sequence would be interpreted as a single string
 * argument (aStringArg=this) and four additional, non-parametric arguments.
 *
 * <b>2. Non-Parametric Arguments</b>
 *
 * Non-parametric arguments specify \e objects that their receiving processes
 * can act on or use in any way. Non-parametric arguments are formalized as:
 *
 * <tt>&lt;arg_value&gt;</tt>
 *
 * where &lt;arg_value&gt; is any sequence of ISO 8859-1 characters. Note that the
 * absence of a <em>standard parameter prefix</em> (the hypen character, '-')
 * characterizes non-parametric arguments. For example, in the following
 * arguments:
 *
 * <tt>-mode="auto" *.js -z foo.scp</tt>
 *
 * '*.js' and 'foo.scp' are two non-parametric arguments.
 *
 * Each non-parametric argument can be expanded into a <em>list of items.</em>
 * For example, the '*.js' argument above would be expanded into a list of all
 * files with the .js suffix in the current directory, provided that the
 * argument is parsed as a wild file path specification.
 *
 * \sa ParseArguments(), ArgumentItemMode, ArgumentOption
 *
 * \ingroup argument_parsing
 */
class PCL_CLASS Argument
{
public:

   /*!
    * Constructs an empty %Argument object.
    */
   Argument()
   {
      Initialize();
   }

   /*!
    * Constructs an %Argument object and parses the specified \a argv string.
    */
   Argument( const String& argv )
   {
      Parse( argv.c_str() );
   }

   /*!
    * Constructs an %Argument object and parses the specified \a argv string.
    */
   Argument( const char16_type* argv )
   {
      Parse( argv );
   }

   /*!
    * Constructs an %Argument object and interprets that the specified \a argv
    * string corresponds to a non-parametric argument corresponding to the
    * \a items list of items.
    */
   Argument( const String& argv, const StringList& items )
   {
      type = item_arg;
      token = argv;
      asItems = items;
   }

   /*!
    * Copy constructor.
    */
   Argument( const Argument& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Argument& operator =( const Argument& ) = default;

   /*!
    * Equality operator. Returns true if this %Argument object is equal to the
    * specified \a x instance.
    */
   bool operator ==( const Argument& x ) const
   {
      if ( id == x.id && type == x.type )
      {
         if ( IsLiteral() || !IsValid() )
            return true;
         if ( IsNumeric() )
            return asNumeric == x.asNumeric;
         if ( IsString() )
            return asString == x.asString;
         if ( IsSwitch() )
            return asSwitch == x.asSwitch;
         if ( IsItemList() )
            return asItems == x.asItems;
      }
      return false;
   }

   /*!
    * Returns true iff this argument has successfully parsed a token string.
    */
   bool IsValid() const
   {
      return type != invalid_arg;
   }

   /*!
    * Returns the token string corresponding to this argument. The token is the
    * exact string that has been parsed by this %Argument instance. If no token
    * has been parsed by this instance, an empty string is returned.
    */
   String Token() const
   {
      return token;
   }

   /*!
    * Returns the argument's parameter identifier, if this argument has been
    * parsed and it is a <em>parametric argument</em>, or an empty string
    * otherwise.
    */
   String Id() const
   {
      return id;
   }

   /*!
    * Returns true iff this argument has been parsed and it is a
    * <em>non-parametric argument</em>, also known as an <em>item
    * argument.</em>
    */
   bool IsItemList() const
   {
      return type == item_arg;
   }

   /*!
    * Returns a reference to the (constant) list of items that have been
    * generated after expansion of a non-parametric argument. The returned list
    * cannot be modified, and it's empty if this argument is parametric, or if
    * it has not been parsed yet.
    */
   const StringList& Items() const
   {
      return asItems;
   }

   /*!
    * Returns a reference to the list of items that have been generated after
    * expansion of a non-parametric argument. The returned list can be freely
    * modified, and it's empty if this argument is parametric, or if it has not
    * been parsed yet.
    */
   StringList& Items()
   {
      return asItems;
   }

   /*!
    * Returns true iff this argument has been parsed and it is a <em>literal
    * argument</em>.
    */
   bool IsLiteral() const
   {
      return type == literal_arg;
   }

   /*!
    * Returns true iff this argument has been parsed and it is a <em>switch
    * argument</em>.
    */
   bool IsSwitch() const
   {
      return type == switch_arg;
   }

   /*!
    * Returns the switch state of this argument, if it has been parsed and it
    * is a <em>switch argument</em>, or false otherwise.
    */
   bool SwitchState() const
   {
      return asSwitch;
   }

   /*!
    * Returns true iff this argument has been parsed and it is a <em>numeric
    * argument</em>.
    */
   bool IsNumeric() const
   {
      return type == numeric_arg;
   }

   /*!
    * Returns the numeric value of this argument, if it has been parsed and it
    * is a <em>numeric argument</em>, or zero otherwise.
    */
   double NumericValue() const
   {
      return asNumeric;
   }

   /*!
    * Returns true iff this argument has been parsed and it is a <em>string
    * argument</em>.
    */
   bool IsString() const
   {
      return type == string_arg;
   }

   /*!
    * Returns the string value of this argument, if it has been parsed and it
    * is a <em>string argument</em>, or an empty string otherwise.
    */
   String StringValue() const
   {
      return asString;
   }

private:

   enum arg_type { invalid_arg = -1, item_arg, literal_arg, switch_arg, numeric_arg, string_arg };

   String      token;      // argument token
   String      id;         // argument id
   arg_type    type;       // argument type, or invalid

   StringList  asItems;    // list of non-parameters (e.g. file names or view ids)
   bool        asSwitch;   // logical state of a switch argument
   double      asNumeric;  // value of a numerical argument
   String      asString;   // value of a string argument

   void Initialize()
   {
      token.Clear();
      id.Clear();
      type = invalid_arg;
      asItems.Clear();
      asSwitch = false;
      asNumeric = 0;
      asString.Clear();
   }

   void Parse( const char16_type* );
};

/*!
 * \class pcl::ArgumentList
 * \brief A dynamic array of command-line arguments
 * \ingroup argument_parsing
 */
typedef Array<Argument> ArgumentList;

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ArgumentItemMode
 * \brief     Non-parametric argument parsing modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ArgumentItemMode::Ignore</td>  <td>Consider non-parametric items as literal items (plain strings)</td></tr>
 * <tr><td>ArgumentItemMode::AsFiles</td> <td>Non-parametric items are file paths</td></tr>
 * <tr><td>ArgumentItemMode::AsViews</td> <td>Non-parametric items are view identifiers</td></tr>
 * <tr><td>ArgumentItemMode::NoItems</td> <td>Non-parametric items are not allowed</td></tr>
 * </table>
 *
 * \ingroup argument_parsing
 */
namespace ArgumentItemMode
{
   enum value_type
   {
      Ignore,  // Consider non-parametric items as literal items (plain strings)
      AsFiles, // Non-parametric items are file paths
      AsViews, // Non-parametric items are view identifiers
      NoItems, // Non-parametric items are not allowed
   };
}

/*!
 * \class pcl::argument_item_mode
 * \brief Represents an ArgumentItemMode enumerated value.
 * \ingroup argument_parsing
 */
typedef ArgumentItemMode::value_type   argument_item_mode;

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ArgumentOption
 * \brief     Working options affecting how non-parametric arguments are
 *            interpreted.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ArgumentOption::AllowWildcards</td>      <td>Allow wildcard characters (*?) in non-parametric items</td></tr>
 * <tr><td>ArgumentOption::NoPreviews</td>          <td>Don't allow preview specifications (with the '->' standard separator)</td></tr>
 * <tr><td>ArgumentOption::RecursiveDirSearch</td>  <td>Perform recursive directory searches for wild path specifications</td></tr>
 * <tr><td>ArgumentOption::RecursiveSearchArgs</td> <td>Use standard arguments to toggle recursive directory searching</td></tr>
 * </table>
 *
 * \ingroup argument_parsing
 */
namespace ArgumentOption
{
   enum mask_type
   {
      AllowWildcards       = 0x00000001, // Allow wildcard characters (*?) in non-parametric items
      NoPreviews           = 0x00000002, // Don't allow preview specifications (with the '->' standard separator)
      RecursiveDirSearch   = 0x00000004, // Perform recursive directory searches for wild path specifications
      RecursiveSearchArgs  = 0x00000008, // Use standard arguments to toggle recursive directory searching
   };
}

/*!
 * \class pcl::ArgumentOptions
 * \brief A combination of ArgumentOption flags
 * \ingroup argument_parsing
 */
typedef Flags<ArgumentOption::mask_type>  ArgumentOptions;

// ----------------------------------------------------------------------------

// The implementation of ExtractArguments() is slightly different in Core.
#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
} // pcl
using namespace pcl;
namespace pi
{
#endif

/*!
 * Extracts a sequence of command-line arguments of the form:
 *
 * <tt>[&lt;arg_list&gt;] [&lt;item_list&gt;]</tt>
 *
 * where:
 *
 * <tt>&lt;item_list&gt;</tt>
 * Is a sequence <tt>&lt;item&gt; [&lt;item_list&gt;]</tt>
 *
 * <tt>&lt;item&gt;</tt>
 * Is a non-parametric (or "item") argument. If <em>file parsing</em> is active
 * (see the \a mode parameter below), each item will be parsed as a file path
 * specification. If <em>view parsing</em> is active, items will be considered
 * as view identifiers. In both cases, each item may include wildcards to
 * define a search pattern, which will be automatically expanded into a
 * sequence of actual file paths or view identifiers.
 *
 * <tt>&lt;arg_list&gt;</tt>
 * Is a sequence <tt>&lt;arg&gt; [&lt;arg_list&gt;]</tt>
 *
 * <tt>&lt;arg&gt;</tt>
 * Is a parametric argument (see the documentation for the Argument class for a
 * complete description).
 *
 * \param argv       The list of input argument tokens that will be parsed.\n\n
 *
 * \param mode       Indicates how non-parametric items will be handled by this
 *          function. \a mode must have one of the following values:\n
 *          \n
 *          <table border="1" cellpadding="4" cellspacing="0">
 *          <tr>
 *          <td><b>ArgumentItemMode::Ignore</b>. Non-parametric arguments will
 *          not be parsed and will be passed unchanged.</td></tr>
 *          <tr>
 *          <td><b>ArgumentItemMode::AsFiles</b>. Each non-parametric argument
 *          is interpreted as a file path specification. Note that neither
 *          validity of file paths nor the existence of actual files are
 *          verified.</td></tr>
 *          <tr>
 *          <td><b>ArgumentItemMode::AsViews</b>. Each non-parametric argument
 *          is interpreted and parsed as a view identifier specification. Note
 *          that only validity of view identifiers is verified, not the
 *          existence of actual views with such identifiers.</td></tr>
 *          <tr>
 *          <td><b>ArgumentItemMode::NoItems</b>. This mode disallows
 *          non-parametric arguments. An exception is thrown if a
 *          non-parametric item is found in the input sequence of \a argv
 *          elements.</td></tr>
 *          </table>\n\n
 *
 * \param options    This is an OR'ed combination of flags from the
 *          ArgumentOption enumeration:\n
 *          \n
 *          <table border="1" cellpadding="4" cellspacing="0">
 *          <tr>
 *          <td><b>ArgumentItemMode::AllowWildcards</b>. If this flag is
 *          included, each file argument containing wildcards ('*' and '?'
 *          characters) will originate a search for all files matching the
 *          specified wildcard pattern. If view arguments are being considered,
 *          each wildcard item will be expanded into a list of matching view
 *          identifiers.</td></tr>
 *          <tr>
 *          <td><b>ArgumentItemMode::NoPreviews</b>. This flag prevents the
 *          inclusion of preview identifiers in the output arguments set. If a
 *          preview specification (imageId->previewId) is found and this flag
 *          is active, an exception is thrown.</td></tr>
 *          <tr>
 *          <td><b>ArgumentItemMode::RecursiveDirSearch</b>. This flag only
 *          makes sense along with the ArgumentItemMode::AsFiles mode. If used,
 *          this flag will originate recursive directory searches for all file
 *          path specifications containing wildcards. Note that the behavior of
 *          this flag can be altered by the RecursiveSearchArgs flag (see
 *          below).</td></tr>
 *          <tr>
 *          <td><b>ArgumentItemMode::RecursiveSearchArgs</b>. This flag only
 *          makes sense along with the ArgumentItemMode::AsFiles mode and the
 *          RecursiveDirSearch flag (see above). If used, recursive directory
 *          searches will be controlled by a special toggle argument. All
 *          instances of this argument will be processed automatically by the
 *          routine and will not be included in the output arguments list. By
 *          default, this argument is <tt>--r[+|-]</tt>, but it can be changed
 *          by calling the SetRecursiveDirSearchArgument() static function.\n
 *          \n
 *          Example of wild path argument specifications with recursive search
 *          through the standard <tt>--r</tt> toggle argument:
 *
 *          <tt>*.cpp --r *.h *.png --r- *.exe</tt>
 *
 *          In the example above, <tt>*.h</tt> and <tt>*.png</tt> will be
 *          searched recursively through the entire directory tree rooted at
 *          the current directory, but <tt>*.cpp</tt> and <tt>*.exe</tt> files
 *          will only be searched for in the current directory.</td></tr>
 *          </table>
 *
 * This function generates a ParseError exception when it encounters an error
 * while parsing the sequence of input arguments.
 *
 * Returns a list with all the arguments extracted from the input \a argv
 * sequence as instances of the Argument class. If non-parametric arguments are
 * being interpreted as file paths, each non-parametric item is expanded to its
 * corresponding full file path. If non-parametric arguments are being parsed
 * as view identifiers, each non-parametric item is replaced (if appropriate)
 * by its full view identifier. Otherwise, non-parametric items are returned
 * unchanged.
 *
 * \ingroup argument_parsing
 */
ArgumentList PCL_FUNC ExtractArguments( const StringList& argv,
                           argument_item_mode mode = ArgumentItemMode::Ignore,
                           ArgumentOptions options = ArgumentOptions() );

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
} // pi
namespace pcl
{
#endif

/*!
 * Returns the identifier of the argument used by ExtractArguments() for
 * automatic toggle of recursive directory searches. By default, this is the
 * <tt>--r[+|-]</tt> argument.
 *
 * \ingroup argument_parsing
 */
String PCL_FUNC RecursiveDirSearchArgument();

/*!
 * Sets the identifier of the argument used by ExtractArguments() for
 * automatic toggle of recursive directory searches.
 *
 * \param id   New identifier for the automatic recursive search argument.
 *             N.B.: When specifying this argument, don't include the '-'
 *             standard parameter prefix.
 *
 * \ingroup argument_parsing
 */
void PCL_FUNC SetRecursiveDirSearchArgument( const String& id );

/*!
 * Auxiliary directory search routine used by ExtractArguments().
 *
 * Returns a list of full file paths corresponding to a template wild file
 * specification (\a filePath), optionally recursing the directory tree,
 * starting from the directory specified in \a filePath.
 *
 * \ingroup argument_parsing
 */
StringList PCL_FUNC SearchDirectory( const String& filePath, bool recursive = false );

/*!
 * Returns a copy of a source string \a s where all references to environment
 * variables have been replaced with their corresponding values.
 *
 * This function finds all occurrences of environment variables of the form:
 *
 * $&lt;env_name&gt;
 *
 * where &lt;env_name&gt; is any sequence of alphabetic, decimal, or underscore
 * characters. If &lt;env_name&gt; corresponds to an existing environment
 * variable of the calling process, the entire $&lt;env_name&gt; sequence is
 * replaced with the variable's value. References to nonexistent environment
 * variables, as well as empty references (isolated $ characters), are
 * replaced with an empty string (removed).
 *
 * This function works recursively: it can replace environment variables
 * inside the values of environment variables. The routine performs
 * replacements recursively until no $ character is found in the string.
 *
 * \ingroup argument_parsing
 */
String PCL_FUNC ReplaceEnvironmentVariables( const String& s );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Arguments_h

// ----------------------------------------------------------------------------
// EOF pcl/Arguments.h - Released 2017-07-18T16:13:52Z
