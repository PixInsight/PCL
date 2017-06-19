//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/ProcessParameter.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_ProcessParameter_h
#define __PCL_ProcessParameter_h

/// \file pcl/ProcessParameter.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Variant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ProcessParameterType
 * \brief %Process parameter data types.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ProcessParameterType::Invalid</td>     <td>Represents an invalid process parameter type</td></tr>
 * <tr><td>ProcessParameterType::UInt8</td>       <td>Unsigned 8-bit integer (unsigned char)</td></tr>
 * <tr><td>ProcessParameterType::Int8</td>        <td>Signed 8-bit integer (char)</td></tr>
 * <tr><td>ProcessParameterType::UInt16</td>      <td>Unsigned 16-bit integer (unsigned short)</td></tr>
 * <tr><td>ProcessParameterType::Int16</td>       <td>Signed 16-bit integer (short)</td></tr>
 * <tr><td>ProcessParameterType::UInt32</td>      <td>Unsigned 32-bit integer (unsigned int)</td></tr>
 * <tr><td>ProcessParameterType::Int32</td>       <td>Signed 32-bit integer (int)</td></tr>
 * <tr><td>ProcessParameterType::UInt64</td>      <td>Unsigned 64-bit integer (unsigned long long)</td></tr>
 * <tr><td>ProcessParameterType::Int64</td>       <td>Signed 64-bit integer (long long)</td></tr>
 * <tr><td>ProcessParameterType::Float</td>       <td>IEEE 754 single precision (float, 32-bit floating point)</td></tr>
 * <tr><td>ProcessParameterType::Double</td>      <td>IEEE 754 double precision (double, 64-bit floating point)</td></tr>
 * <tr><td>ProcessParameterType::Boolean</td>     <td>Boolean (implemented as uint32, where true=1 and false=0)</td></tr>
 * <tr><td>ProcessParameterType::Enumeration</td> <td>Enumerated (implemented as int32)</td></tr>
 * <tr><td>ProcessParameterType::String</td>      <td>String (Unicode string encoded as UTF-16)</td></tr>
 * <tr><td>ProcessParameterType::Block</td>       <td>Block (implemented as ByteArray)</td></tr>
 * <tr><td>ProcessParameterType::Table</td>       <td>Table process parameter</td></tr>
 * </table>
 */
namespace ProcessParameterType
{
   enum value_type
   {
      Invalid = -1,
      UInt8 = 0,
      Int8,
      UInt16,
      Int16,
      UInt32,
      Int32,
      UInt64,
      Int64,
      Float,
      Double,
      Boolean,
      Enumeration,
      String,
      Block,
      Table,
      NumberOfTypes
   };

   /*!
    * \defgroup process_parameter_classification Process Parameter&nbsp;\
    * Classification
    */

   /*!
    * Returns true ifff the specified \a type corresponds to a numeric
    * parameter data type.
    * \ingroup process_parameter_classification
    */
   inline bool IsNumeric( int type )
   {
      return type >= UInt8 && type <= Double;
   }

   /*!
    * Returns true ifff the specified \a type corresponds to an integer
    * parameter data type.
    * \ingroup process_parameter_classification
    */
   inline bool IsInteger( int type )
   {
      return type >= UInt8 && type <= Int64;
   }

   /*!
    * Returns true ifff the specified \a type corresponds to a floating point
    * real parameter data type.
    * \ingroup process_parameter_classification
    */
   inline bool IsReal( int type )
   {
      return type == Float || type == Double;
   }

   /*!
    * Returns true ifff the specified \a type corresponds to a variable-length
    * parameter data type.
    * \ingroup process_parameter_classification
    */
   inline bool IsVariableLength( int type )
   {
      return type == String || type == Block || type == Table;
   }
}

// ----------------------------------------------------------------------------

class Process;
class ProcessParameterPrivate;

/*!
 * \class ProcessParameter
 * \brief Identifies and describes a process parameter.
 *
 * The %ProcessParameter class describes and identifies process parameters
 * through intermodule communication with the Process and ProcessInstance
 * classes.
 *
 * As happens with the Process / MetaProcess and ProcessInstance /
 * ProcessImplementation pairs, %ProcessParameter is the concrete counterpart
 * to MetaParameter. %ProcessParameter represents a parameter of an already
 * installed process, while %MetaParameter is a formal description of a
 * parameter of a process implemented by subclassing %MetaProcess and
 * %ProcessImplementation.
 *
 * Note that an instance of %ProcessParameter represents an existing parameter
 * of an installed process, so you cannot create or define new parameters using
 * this class. You can only use this class to get a complete description of a
 * parameter, including its identifier, range of valid values, allowed lengths,
 * default values, etc. You can also use %ProcessParameter to identify a
 * parameter to get or set its current value in an existing process instance,
 * which you can manipulate with the ProcessInstance class.
 *
 * \sa Process, ProcessInstance, MetaProcess, MetaParameter, ProcessImplementation
 */
class PCL_CLASS ProcessParameter
{
public:

   /*!
    * Represents a parameter data type.
    */
   typedef ProcessParameterType::value_type  data_type;

   /*!
    * A list of process parameters.
    *
    * This type is returned by the TableColumns() member function to describe
    * and identify the set of column parameters of a table process parameter.
    */
   typedef Array<ProcessParameter>  parameter_list;

   /*!
    * \struct EnumerationElement
    * \brief Structure used to describe an enumeration element.
    */
   struct EnumerationElement
   {
      IsoString     id;      //!< Element identifier
      IsoStringList aliases; //!< Alias identifiers
      int           value;   //!< Element value
   };

   /*!
    * A list of enumeration elements.
    *
    * This type is returned by the EnumerationElements() member function to
    * describe an enumeration process parameter.
    */
   typedef Array<EnumerationElement>   enumeration_element_list;

   /*!
    * Constructs a %ProcessParameter object.
    *
    * \param process    Reference to a the process from which you want to gain
    *                   access to one of its parameters.
    *
    * \param paramId    Identifier of the process parameter. It can be the
    *                   primary identifier of a parameter, or one of its alias
    *                   identifiers.
    *
    * If successful, this constructor creates a managed alias to a process
    * parameter of an installed process in the PixInsight core application.
    *
    * If there is no parameter of \a process with the specified identifier, if
    * \a paramId is not a valid identifier, or if the alias object cannot be
    * generated for some reason, this constructor throws an Error exception
    * with the corresponding error message. Your code should guarantee that
    * these exceptions are always caught and handled appropriately.
    *
    * \note Note that this constructor does not 'create' a new process
    * parameter. It only gives access to a parameter of an existing process.
    */
   ProcessParameter( const Process& process, const IsoString& paramId );

   ProcessParameter( const Process& process, const IsoString::ustring_base& paramId ) :
      ProcessParameter( process, IsoString( paramId ) )
   {
   }

   /*!
    * Constructs a %ProcessParameter object that identifies a table column
    * parameter.
    *
    * \param table      Reference to a valid table parameter.
    *
    * \param colId      Identifier of the column parameter. It can be the
    *                   primary identifier of a column parameter, or one of its
    *                   alias identifiers.
    *
    * If successful, this constructor creates a managed alias to a table column
    * parameter of an installed process in the PixInsight core application.
    *
    * If there is no column parameter of \a table with the specified
    * identifier, if \a table is not a table parameter, if \a colId is not a
    * valid identifier, or if the alias object cannot be generated for some
    * reason, this constructor throws an Error exception with the corresponding
    * error message. Your code should guarantee that these exceptions are
    * always caught and handled appropriately.
    *
    * \note Note that this constructor does not 'create' a new process
    * parameter. It only gives access to a parameter of an existing process.
    */
   ProcessParameter( const ProcessParameter& table, const IsoString& colId );

   ProcessParameter( const ProcessParameter& table, const IsoString::ustring_base& colId ) :
      ProcessParameter( table, IsoString( colId ) )
   {
   }

   /*!
    * Copy constructor. The newly constructed object references the same
    * server-side process parameter as the specified object \a p.
    *
    * \note Note that this constructor does not 'create' a new process
    * parameter. It only gives access to a parameter of an existing process.
    */
   ProcessParameter( const ProcessParameter& p );

   /*!
    * Destroys this %ProcessParameter object.
    */
   virtual ~ProcessParameter();

   /*!
    * Returns true iff this is a \e null %ProcessParameter object.
    *
    * A null %ProcessParameter does not correspond to an existing parameter of
    * an installed process in the PixInsight core application.
    */
   bool IsNull() const;

   /*!
    * Returns a reference to a null %ProcessParameter object. A null
    * %ProcessParameter does not correspond to an existing parameter of an
    * installed process in the PixInsight core application.
    */
   static ProcessParameter& Null();

   /*!
    * Returns a reference to a Process object that represents the installed
    * process this parameter belongs to.
    */
   Process& ParentProcess() const;

   /*!
    * If this parameter is a table column parameter, this function returns a
    * %ProcessParameter object that represents the table process parameter this
    * table column belongs to.
    *
    * If this parameter is not a table column parameter, this function returns
    * a null %ProcessParameter object.
    */
   ProcessParameter ParentTable() const;

   /*!
    * Returns the identifier of this process parameter.
    *
    * Each process parameter has a valid and unique (within its parent process)
    * identifier. All object identifiers follow the C syntax rules for variable
    * identifiers in PixInsight.
    *
    * \sa Aliases()
    */
   IsoString Id() const;

   /*!
    * Returns a list of alias identifiers for this process parameter.
    *
    * A process parameter can have one or more <em>alias identifiers</em>.
    * Aliased parameters are useful to maintain compatibility with previous
    * versions of a process.
    *
    * When the PixInsight core application imports a process instance (e.g.
    * from a process icon) it automatically replaces alias parameter
    * identifiers with actual (current) identifiers. This allows a developer to
    * change the identifiers of some process parameters without breaking
    * compatibility with process instances in existing icon collections or
    * projects.
    *
    * This function returns a list of identifiers that are treated as synonyms
    * to the actual identifier of this parameter, which is returned by the Id()
    * member function. If this parameter has no aliases, an empty list is
    * returned. Two parameters of a process can't have the same alias, so each
    * identifier in the list should be unique within its parent process.
    *
    * \sa Id()
    */
   IsoStringList Aliases() const;

   /*!
    * Returns true iff this process parameter is required to build new process
    * instances. Returns false if a default value can be valid for this process
    * parameter when an actual value is not available.
    *
    * When a new instance is being constructed and the process parameter that
    * this object represents is not present (for example, because it is not
    * defined in a serialized representation of the instance), the PixInsight
    * core application has to decide whether a default, automatically generated
    * value may be acceptable, or if the explicit presence of an actual value
    * for this parameter is imprescindible. This member function allows a
    * module to know this in advance.
    */
   bool IsRequired() const;

   /*!
    * Returns true if this object represents a read-only process parameter.
    * Otherwise full read/write access is granted for this parameter.
    *
    * Read-only process parameters, also known as <em>output properties</em>,
    * cannot be changed by external processes. For example, a script running on
    * the core application can retrieve an output property from a process
    * instance, but it cannot change its value. The same happens with PCL's
    * implementation represented by the Process and ProcessInstance classes.
    *
    * Read-only parameters are often used to provide resulting values or
    * process status information after instance execution. For example, the
    * standard ImageIntegration process includes several read-only parameters
    * that can be accessed from a script or module to know the identifiers of
    * the views generated to store the integrated image and the rejection map
    * images. This improves versatility and usability of processes, and
    * provides more accurate and extensive information to developers.
    */
   bool IsReadOnly() const;

   /*!
    * Returns a brief (and hopefully useful) description of this process
    * parameter.
    */
   String Description() const;

   /*!
    * Returns a script comment for this process parameter.
    *
    * The returned string is intended to be used as a source code comment to
    * document process parameters in automatically generated scripts.
    */
   String ScriptComment() const;

   /*!
    * Returns the data type of this process parameter. For a list of valid
    * parameter types, see the ProcessParameterType namespace.
    *
    * For a null %ProcessParameter object, this member function returns
    * ProcessParameterType::Invalid.
    */
   data_type Type() const;

   /*!
    * Returns the appropriate data type to interpret this block process
    * parameter.
    *
    * If this object does not represent a block parameter, or if this is a null
    * object, this member function returns ProcessParameterType::Invalid.
    */
   data_type DataInterpretation() const;

   /*!
    * Returns true iff this object represents a Boolean process parameter.
    */
   bool IsBoolean() const
   {
      return Type() == ProcessParameterType::Boolean;
   }

   /*!
    * Returns true iff this object represents a numeric process parameter.
    */
   bool IsNumeric() const
   {
      return ProcessParameterType::IsNumeric( Type() );
   }

   /*!
    * Returns true iff this object represents an integer process parameter.
    */
   bool IsInteger() const
   {
      return ProcessParameterType::IsInteger( Type() );
   }

   /*!
    * Returns true iff this object represents a floating point real process
    * parameter.
    */
   bool IsReal() const
   {
      return ProcessParameterType::IsReal( Type() );
   }

   /*!
    * Returns true iff this object represents an enumerated process parameter.
    */
   bool IsEnumeration() const
   {
      return Type() == ProcessParameterType::Enumeration;
   }

   /*!
    * Returns true iff this object represents a variable-length process
    * parameter.
    */
   bool IsVariableLength() const
   {
      return ProcessParameterType::IsVariableLength( Type() );
   }

   /*!
    * Returns true iff this object represents a string process parameter.
    */
   bool IsString() const
   {
      return Type() == ProcessParameterType::String;
   }

   /*!
    * Returns true iff this object represents a block process parameter.
    */
   bool IsBlock() const
   {
      return Type() == ProcessParameterType::Block;
   }

   /*!
    * Returns true iff this object represents a table process parameter.
    */
   bool IsTable() const
   {
      return Type() == ProcessParameterType::Table;
   }

   /*!
    * Returns the default value of this process parameter.
    *
    * This function returns a Variant object with the default parameter value.
    * The type of the %Variant object depends on the type of this parameter.
    *
    * Default values are directly available for Boolean, numeric and string
    * parameters.
    *
    * For enumerated parameters, this function returns the index of the default
    * enumeration element. The returned integer can be used as an index on the
    * list of elements returned by EnumerationElements().
    *
    * For a block parameter an empty ByteArray is always returned, since block
    * parameters don't have default values.
    *
    * For table parameters this function throws an Error exception.
    *
    * For a null %ProcessParameter object, this member function returns an
    * invalid Variant object; see Variant::IsValid().
    */
   Variant DefaultValue() const;

   /*!
    * Gets the valid range of values for a numeric process parameter.
    *
    * \param[out] minValue  Minimum valid value of this numeric parameter.
    * \param[out] maxValue  Maximum valid value of this numeric parameter.
    *
    * If this parameter is not of a numeric type, or if this is a null object,
    * this member function will set both output values to zero.
    *
    * If this numeric parameter does not impose a specific range of valid
    * values, this member function will set the following output values:
    *
    * \a minValue = -DBL_MAX \n
    * \a maxValue = +DBL_MAX.
    *
    * \sa MinimumValue(), MaximumValue()
    */
   void GetNumericRange( double& minValue, double& maxValue ) const;

   /*!
    * Returns the minimum valid value of a numeric process parameter.
    * \sa GetNumericRange(), MaximumValue()
    */
   double MinimumValue() const
   {
      double min, dum;
      GetNumericRange( min, dum );
      return min;
   }

   /*!
    * Returns the maximum valid value of a numeric process parameter.
    * \sa GetNumericRange(), MinimumValue()
    */
   double MaximumValue() const
   {
      double dum, max;
      GetNumericRange( dum, max );
      return max;
   }

   /*!
    * Returns the number of decimal digits used for literal representations of
    * this floating point process parameter.
    *
    * If this parameter is not of a floating point numeric type, or if this is
    * a null object, this function returns zero. Otherwise, this function
    * returns a positive or negative integer value with the following meanings:
    *
    * If a value >= 0 is returned, literal representations are always rounded
    * to the nearest value with the specified amount of decimal digits. If this
    * function returns zero, output values corresponding to this parameter will
    * always be rounded to the nearest integer.
    *
    * If this function returns a negative integer, literal representations will
    * use the standard printf's \e g-format. In this format, numerical values
    * are represented in a compact fashion, with automatic selection of
    * exponential or fixed representations, depending on the value's magnitude
    * and fractional accuracy.
    *
    * \sa ScientificNotation()
    */
   int Precision() const;

   /*!
    * Returns true iff this floating point process parameter requires scientific
    * notation for literal representations. Returns false if this parameter
    * requires fixed point notation, or if this is a null object.
    *
    * \sa Precision()
    */
   bool ScientificNotation() const;

   /*!
    * Gets the valid range of lengths for this variable length process
    * parameter.
    *
    * \param[out] minLength   Minimum valid length of this parameter.
    * \param[out] maxLength   Maximum valid length of this parameter.
    *
    * If this parameter is not of a variable length type, or if this is a null
    * object, this member function will set both output values to zero.
    *
    * If this parameter does not impose a specific range of valid lengths, this
    * member function will set the following output values:
    *
    * \a minValue = 0 \n
    * \a maxValue = ~size_type( 0 ).
    *
    * \sa MinimumLength(), MaximumLength()
    */
   void GetLengthLimits( size_type& minLength, size_type& maxLength ) const;

   /*!
    * Returns the minimum valid length of a variable length process parameter.
    * \sa GetLengthLimits(), MaximumLength()
    */
   size_type MinimumLength() const
   {
      size_type min, dum;
      GetLengthLimits( min, dum );
      return min;
   }

   /*!
    * Returns the maximum valid length of a variable length process parameter.
    * \sa GetLengthLimits(), MinimumLength()
    */
   size_type MaximumLength() const
   {
      size_type dum, max;
      GetLengthLimits( dum, max );
      return max;
   }

   /*!
    * Returns a list of EnumerationElement structures describing all the
    * elements of this enumeration process parameter.
    *
    * If this parameter is not of an enumerated type, or if this is a null
    * object, this member function returns an empty list.
    */
   enumeration_element_list EnumerationElements() const;

   /*!
    * Returns a string with the valid characters of this string process
    * parameter.
    *
    * If this parameter is not of a string type, if this string parameter does
    * not impose a specific set of valid characters, or if this is a null
    * object, this member function returns an empty string.
    */
   String AllowedCharacters() const;

   /*!
    * Returns the list of column parameters in this table process parameter.
    *
    * If this object does not represent a table parameter, or if this is a null
    * object, this member function returns an empty list.
    */
   parameter_list TableColumns() const;

   // -------------------------------------------------------------------------

private:

   AutoPointer<ProcessParameterPrivate> m_data;

   ProcessParameter( const void* );

   const void* Handle() const;

   friend class ProcessInstance;
   friend class InternalParameterEnumerator;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ProcessParameter_h

// ----------------------------------------------------------------------------
// EOF pcl/ProcessParameter.h - Released 2017-06-17T10:55:43Z
