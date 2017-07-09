//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/MetaParameter.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_MetaParameter_h
#define __PCL_MetaParameter_h

/// \file pcl/MetaParameter.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/MetaObject.h>
#include <pcl/String.h>

#include <float.h> // DBL_MAX

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS MetaProcess;
class PCL_CLASS MetaTable;

// ----------------------------------------------------------------------------

/*!
 * \class MetaParameter
 * \brief Root base class for PCL classes providing formal descriptions of
 *        process parameters.
 *
 * %MetaParameter represents a <em>process parameter</em>. This is an abstract
 * base class providing fundamental functionality common to all process
 * parameters available in PCL. Derived classes implement and extended that
 * functionality to represent a wide variety of process parameter types.
 *
 * %Process parameters can be numerical values (MetaInteger and MetaReal, and
 * their derived classes with different ranges), Boolean values (MetaBoolean),
 * enumerated types (MetaEnumeration), and dynamic strings (MetaString).
 *
 * In addition, a set of arbitrary process parameters can be organized to
 * define the columns of a <em>table parameter</em> (MetaTable). A table
 * parameter can have an unlimited, variable number of rows.
 *
 * Finally, <em>block parameters</em> (MetaBlock) can also be defined to store
 * arbitrary data as memory blocks of variable size.
 *
 * \sa MetaProcess, ProcessImplementation
 */
class PCL_CLASS MetaParameter : public MetaObject
{
public:

   /*!
    * Constructs a metaparameter representing a parameter of the specified
    * process class \a *P.
    */
   MetaParameter( MetaProcess* P );

   /*!
    * Constructs a metaparameter representing a column of a table process
    * parameter represented by the metatable \a *T. The new metaparameter is
    * appended to the list of existing columns in the metatable \a *T.
    */
   MetaParameter( MetaTable* T );

   /*!
    * Destroys a %MetaParameter object.
    */
   virtual ~MetaParameter() noexcept( false )
   {
   }

   /*!
    * Returns the identifier of the process parameter that this metaparameter
    * represents.
    *
    * Each process parameter must have a valid, unique (within its parent
    * process) C identifier.
    *
    * \sa Aliases()
    */
   virtual IsoString Id() const = 0;

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
    * The returned string is a comma-separated list of identifiers that will be
    * treated as synonyms to the actual identifier of this parameter, which is
    * returned by the Id() member function. Two parameters of a process can't
    * have the same alias, so each identifier included in the list must be
    * unique within its parent process.
    *
    * \note The default implementation of this member function returns an empty
    * string (no aliases).
    *
    * \sa Id()
    */
   virtual IsoString Aliases() const
   {
      return IsoString();
   }

   /*!
    * Returns true iff actual process parameters represented by this
    * metaparameter require <em>explicit unlock calls</em>.
    *
    * An explicit unlock is a call to ProcessImplementation::UnlockParameter(),
    * which always happens after an initial call to
    * ProcessImplementation::LockParameter().
    *
    * \note    The default implementation of this function returns false, so a
    * process parameter is not explicitly unlocked by default.
    */
   virtual bool NeedsUnlocking() const
   {
      return false;
   }

   /*!
    * Returns true iff actual process parameters represented by this
    * metaparameter require <em>explicit validation</em>.
    *
    * An explicit validation occurs when the PixInsight Core application
    * performs a call to ProcessImplementation::ValidateParameter(), either as
    * part of a process instance execution, or indirectly because a module has
    * requested it by calling ProcessInstance::Validate().
    *
    * For example, suppose that you define an integer parameter for a process.
    * The MetaInteger class is a descendant of MetaNumeric, which allows you to
    * define a valid range of values. This is sufficient to automatically
    * reject any actual parameter value outside the declared range. But suppose
    * that only \e odd integer values (1, 3, 5, 7 ...) are valid values for the
    * parameter in question. The only way to impose such restriction is
    * reimplementing this function to return true, and the corresponding
    * ProcessImplementation::ValidateParameter() function to reject any even
    * integer value.
    *
    * \note    The default implementation of this function returns false, so a
    * process parameter is never validated by default, unless a module
    * explicitly requests validation of a process instance.
    */
   virtual bool NeedsValidation() const
   {
      return false;
   }

   // ### Undocumented
   virtual uint32 FirstProcessVersion() const
   {
      return 0;
   }

   // ### Undocumented
   virtual uint32 LastProcessVersion() const
   {
      return 0;
   }

   /*!
    * Returns true iff the process parameter represented by this metaparameter
    * is required to build new process instances. Returns false if a default
    * value can be valid for the process parameter when an actual value is not
    * available.
    *
    * When a new instance is being constructed and the represented process
    * parameter is not present, this function is called to decide whether a
    * default, automatically generated value may be acceptable, or if the
    * explicit presence of an actual value for the represented parameter is
    * imprescindible.
    *
    * For example, if you implement a new version of an existing process,
    * perhaps you will define new parameters as well. The new parameters will
    * not be available in existing stored instances (e.g., in PSM files, or
    * scripts) of previous versions. In this case, if you can provide plausible
    * default values for the new parameters, and you don't reimplement this
    * function for them, then the old process instances will still be reusable.
    *
    * \note    The default implementation of this function returns false, so
    * actual parameter values are not strictly required by default.
    */
   virtual bool IsRequired() const
   {
      return false;
   }

   /*!
    * Returns true if this metaparameter represents a read-only process
    * parameter. Otherwise full read/write access to this parameter is granted
    * for external processes.
    *
    * Read-only parameters or \e properties cannot be changed by external
    * processes. For example, a script running on the core application can
    * retrieve a read-only property from a process instance, but it cannot
    * change its value.
    *
    * Read-only properties are often used to provide resulting values or
    * process status information after instance execution. For example, the
    * standard ImageIntegration process includes several read-only properties
    * that can be accessed from a script to know the identifiers of the views
    * generated to store the integrated image and the rejection map images.
    * This improves versatility and usability of processes from scripts, and
    * provides more accurate and extensive information to the users.
    *
    * \note    The default implementation of this function returns false, so
    * process parameters allow normal read/write access by default.
    */
   virtual bool IsReadOnly() const
   {
      return false;
   }

   /*!
    * Returns a brief description of the process parameter represented by this
    * metaparameter.
    *
    * The returned string will be used on the %Process Explorer window, which
    * gives a summary of the parameters of each installed process. The
    * information will be written to a console, so you can use the whole
    * functionality of Console to format the text given here.
    *
    * Try to write succint but meaningful parameter descriptions.
    */
   virtual String Description() const
   {
      return String();
   }

   /*!
    * Returns a script comment for the process parameter represented by this
    * metaparameter.
    *
    * For the sake of compatibility, the returned string should contain only
    * valid ISO-8859-1 characters (represented as UTF-16 since the return value
    * is a String object). No special characters, control tags or character
    * entities are supported. This string will be used as a source code comment
    * to document process parameters in automatically generated scripts.
    *
    * If this member function is not reimplemented for a table parameter (see
    * MetaTable), PCL automatically generates a script comment including the
    * identifiers of all table column parameters.
    */
   virtual String Comment() const
   {
      return String();
   }

   /*!
    * Returns true iff this metaparameter represents a numeric process
    * parameter. Numeric process parameters are represented as subclasses of
    * MetaNumeric.
    */
   virtual bool IsNumeric() const
   {
      return false;
   }

   /*!
    * Returns true iff this metaparameter represents a Boolean process
    * parameter. Boolean process parameters are represented as subclasses of
    * MetaBoolean.
    */
   virtual bool IsBoolean() const
   {
      return false;
   }

   /*!
    * Returns true iff this metaparameter represents an enumerated process
    * parameter. Enumerated process parameters are represented as subclasses of
    * MetaEnumeration.
    */
   virtual bool IsEnumeration() const
   {
      return false;
   }

   /*!
    * Returns true iff this metaparameter represents a variable-length process
    * parameter. Variable-length process parameters are represented as
    * subclasses of MetaVariableLengthParameter.
    */
   virtual bool IsVariableLength() const
   {
      return false;
   }

   /*!
    * Returns true iff this metaparameter represents a string process parameter.
    * %String process parameters are represented as subclasses of MetaString.
    */
   virtual bool IsString() const
   {
      return false;
   }

   /*!
    * Returns true iff this metaparameter represents a table process parameter.
    * Table process parameters are represented as subclasses of MetaTable.
    */
   virtual bool IsTable() const
   {
      return false;
   }

   /*!
    * Returns true iff this metaparameter represents a block process parameter.
    * Block process parameters are represented as subclasses of MetaBlock.
    */
   virtual bool IsBlock() const
   {
      return false;
   }

protected:

   virtual void PerformAPIDefinitions() const;

   virtual void PerformTypeAPIDefinitions() const
   {
   }

   virtual uint32 APIParType() const = 0;

   static bool inTableDefinition;

   friend class MetaTable;
   friend class MetaProcess;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaNumeric
 * \brief A formal description of a numeric process parameter.
 *
 * Numeric process parameters can be integers (MetaInteger) and floating-point
 * reals (MetaReal).
 *
 * Integer process parameters can be signed (MetaSignedInteger) and unsigned
 * (MetaUnsignedInteger) 8, 16, 32 and 64-bit integers. Real process parameters
 * can be 32 and 64-bit IEEE 754 floating point values. This leads to the
 * following numeric metaparameter classes:
 *
 * \li Unsigned integer process parameters: MetaUInt8, MetaUInt16, MetaUInt32,
 * and MetaUInt64.
 *
 * \li Signed integer process parameters: MetaInt8, MetaInt16, MetaInt32, and
 * MetaInt64.
 *
 * \li Real process parameters: MetaFloat and MetaDouble.
 */
class PCL_CLASS MetaNumeric : public MetaParameter
{
public:

   /*!
    * Constructs a metaparameter representing a numeric parameter of the
    * specified process class \a *P.
    */
   MetaNumeric( MetaProcess* P ) : MetaParameter( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a numeric value in a column of a
    * table process parameter represented by the metatable \a *T. The new
    * metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaNumeric( MetaTable* T ) : MetaParameter( T )
   {
   }

   /*!
    * Destroys a %MetaNumeric object.
    */
   virtual ~MetaNumeric() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsNumeric() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns true iff this metaparameter represents a real (floating-point)
    * process parameter. Real process parameters are represented as subclasses
    * of MetaReal.
    */
   virtual bool IsReal() const
   {
      return false;
   }

   /*!
    * Returns true iff this metaparameter represents an integer process
    * parameter. Integer process parameters are represented as subclasses of
    * MetaReal.
    */
   virtual bool IsInteger() const
   {
      return false;
   }

   /*!
    * Returns the default value of the numeric process parameter represented by
    * this metaparameter.
    *
    * \note    The default implementation of this function returns zero.
    */
   virtual double DefaultValue() const
   {
      return 0;
   }

   /*!
    * Returns the minimum valid value for the numeric process parameter
    * represented by this metaparameter.
    *
    * \note    The default implementation of this function returns -DBL_MAX,
    * which is the minimum possible value of a \c double number.
    */
   virtual double MinimumValue() const
   {
      return -DBL_MAX;
   }

   /*!
    * Returns the maximum valid value for the numeric process parameter
    * represented by this metaparameter.
    *
    * \note    The default implementation of this function returns DBL_MAX,
    * which is the maximum possible value of a \c double number.
    */
   virtual double MaximumValue() const
   {
      return +DBL_MAX;
   }

protected:

   virtual void PerformTypeAPIDefinitions() const;
   virtual uint32 APIParType() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaInteger
 * \brief A formal description of an integer process parameter.
 *
 * Integer process parameters can be signed (MetaSignedInteger) and unsigned
 * (MetaUnsignedInteger) 8, 16, 32 and 64-bit integers. This leads to the
 * following integer process parameter classes:
 *
 * \li Unsigned integer process parameters: MetaUInt8, MetaUInt16, MetaUInt32,
 * and MetaUInt64.
 *
 * \li Signed integer process parameters: MetaInt8, MetaInt16, MetaInt32, and
 * MetaInt64.
 */
class PCL_CLASS MetaInteger : public MetaNumeric
{
public:

   /*!
    * Constructs a metaparameter representing an integer parameter of the
    * specified process class \a *P.
    */
   MetaInteger( MetaProcess* P ) : MetaNumeric( P )
   {
   }

   /*!
    * Constructs a metaparameter representing an integer value in a column of a
    * table process parameter represented by the metatable \a *T. The new
    * metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaInteger( MetaTable* T ) : MetaNumeric( T )
   {
   }

   /*!
    * Destroys a %MetaInteger object.
    */
   virtual ~MetaInteger() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsInteger() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns true if this metaparameter represents a signed integer process
    * parameter; false if it represents an unsigned integer process parameter.
    */
   virtual bool IsSigned() const = 0;

private:

   virtual uint32 APIParType() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaUnsignedInteger
 * \brief A formal description of an unsigned integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit unsigned integer process parameters. This
 * leads to the following unsigned process parameter classes: MetaUInt8,
 * MetaUInt16, MetaUInt32, and MetaUInt64.
 */
class PCL_CLASS MetaUnsignedInteger : public MetaInteger
{
public:

   /*!
    * Constructs a metaparameter representing an unsigned integer parameter of
    * the specified process class \a *P.
    */
   MetaUnsignedInteger( MetaProcess* P ) : MetaInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing an unsigned integer value in a
    * column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaUnsignedInteger( MetaTable* T ) : MetaInteger( T )
   {
   }

   /*!
    * Destroys a %MetaUnsignedInteger object.
    */
   virtual ~MetaUnsignedInteger() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsSigned() const
   {
      return false;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaSignedInteger
 * \brief A formal description of a signed integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit signed integer process parameters. This
 * leads to the following signed process parameter classes: MetaInt8,
 * MetaInt16, MetaInt32, and MetaInt64.
 */
class PCL_CLASS MetaSignedInteger : public MetaInteger
{
public:

   /*!
    * Constructs a metaparameter representing a signed integer parameter of
    * the specified process class \a *P.
    */
   MetaSignedInteger( MetaProcess* P ) : MetaInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a signed integer value in a
    * column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaSignedInteger( MetaTable* T ) : MetaInteger( T )
   {
   }

   /*!
    * Destroys a %MetaSignedInteger object.
    */
   virtual ~MetaSignedInteger() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsSigned() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaUInt8
 * \brief A formal description of an 8-bit unsigned integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit unsigned integer process parameters. This
 * leads to the following unsigned process parameter classes: MetaUInt8,
 * MetaUInt16, MetaUInt32, and MetaUInt64.
 */
class PCL_CLASS MetaUInt8 : public MetaUnsignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing an 8-bit unsigned integer
    * parameter of the specified process class \a *P.
    */
   MetaUInt8( MetaProcess* P ) : MetaUnsignedInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing an 8-bit unsigned integer value
    * in a column of a table process parameter represented by the metatable
    * \a *T. The new metaparameter is appended to the list of existing columns
    * in the metatable \a *T.
    */
   MetaUInt8( MetaTable* T ) : MetaUnsignedInteger( T )
   {
   }

   /*!
    * Destroys a %MetaUInt8 object.
    */
   virtual ~MetaUInt8() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaUInt16
 * \brief A formal description of a 16-bit unsigned integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit unsigned integer process parameters. This
 * leads to the following unsigned process parameter classes: MetaUInt8,
 * MetaUInt16, MetaUInt32, and MetaUInt64.
 */
class PCL_CLASS MetaUInt16 : public MetaUnsignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing a 16-bit unsigned integer
    * parameter of the specified process class \a *P.
    */
   MetaUInt16( MetaProcess* P ) : MetaUnsignedInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a 16-bit unsigned integer value
    * in a column of a table process parameter represented by the metatable
    * \a *T. The new metaparameter is appended to the list of existing columns
    * in the metatable \a *T.
    */
   MetaUInt16( MetaTable* T ) : MetaUnsignedInteger( T )
   {
   }

   /*!
    * Destroys a %MetaUInt16 object.
    */
   virtual ~MetaUInt16() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaUInt32
 * \brief A formal description of a 32-bit unsigned integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit unsigned integer process parameters. This
 * leads to the following unsigned process parameter classes: MetaUInt8,
 * MetaUInt16, MetaUInt32, and MetaUInt64.
 */
class PCL_CLASS MetaUInt32 : public MetaUnsignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing a 32-bit unsigned integer
    * parameter of the specified process class \a *P.
    */
   MetaUInt32( MetaProcess* P ) : MetaUnsignedInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a 32-bit unsigned integer value
    * in a column of a table process parameter represented by the metatable
    * \a *T. The new metaparameter is appended to the list of existing columns
    * in the metatable \a *T.
    */
   MetaUInt32( MetaTable* T ) : MetaUnsignedInteger( T )
   {
   }

   /*!
    * Destroys a %MetaUInt32 object.
    */
   virtual ~MetaUInt32() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaUInt64
 * \brief A formal description of a 64-bit unsigned integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit unsigned integer process parameters. This
 * leads to the following unsigned process parameter classes: MetaUInt8,
 * MetaUInt16, MetaUInt32, and MetaUInt64.
 */
class PCL_CLASS MetaUInt64 : public MetaUnsignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing a 64-bit unsigned integer
    * parameter of the specified process class \a *P.
    */
   MetaUInt64( MetaProcess* P ) : MetaUnsignedInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a 64-bit unsigned integer value
    * in a column of a table process parameter represented by the metatable
    * \a *T. The new metaparameter is appended to the list of existing columns
    * in the metatable \a *T.
    */
   MetaUInt64( MetaTable* T ) : MetaUnsignedInteger( T )
   {
   }

   /*!
    * Destroys a %MetaUInt64 object.
    */
   virtual ~MetaUInt64() noexcept( false )
   {
   }

   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaInt8
 * \brief A formal description of an 8-bit signed integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit signed integer process parameters. This
 * leads to the following signed process parameter classes: MetaInt8,
 * MetaInt16, MetaInt32, and MetaInt64.
 */
class PCL_CLASS MetaInt8 : public MetaSignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing an 8-bit signed integer parameter
    * of the specified process class \a *P.
    */
   MetaInt8( MetaProcess* P ) : MetaSignedInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing an 8-bit signed integer value in
    * a column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaInt8( MetaTable* T ) : MetaSignedInteger( T )
   {
   }

   /*!
    * Destroys a %MetaInt8 object.
    */
   virtual ~MetaInt8() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaInt16
 * \brief A formal description of a 16-bit signed integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit signed integer process parameters. This
 * leads to the following signed process parameter classes: MetaInt8,
 * MetaInt16, MetaInt32, and MetaInt64.
 */
class MetaInt16 : public MetaSignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing a 16-bit signed integer parameter
    * of the specified process class \a *P.
    */
   MetaInt16( MetaProcess* P ) : MetaSignedInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a 16-bit signed integer value in
    * a column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaInt16( MetaTable* T ) : MetaSignedInteger( T )
   {
   }

   /*!
    * Destroys a %MetaInt16 object.
    */
   virtual ~MetaInt16() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaInt32
 * \brief A formal description of a 32-bit signed integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit signed integer process parameters. This
 * leads to the following signed process parameter classes: MetaInt8,
 * MetaInt16, MetaInt32, and MetaInt64.
 */
class PCL_CLASS MetaInt32 : public MetaSignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing a 32-bit signed integer parameter
    * of the specified process class \a *P.
    */
   MetaInt32( MetaProcess* P ) : MetaSignedInteger( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a 32-bit signed integer value in
    * a column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaInt32( MetaTable* T ) : MetaSignedInteger( T )
   {
   }

   /*!
    * Destroys a %MetaInt32 object.
    */
   virtual ~MetaInt32() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaInt64
 * \brief A formal description of a 64-bit signed integer process parameter.
 *
 * PCL supports 8, 16, 32 and 64-bit signed integer process parameters. This
 * leads to the following signed process parameter classes: MetaInt8,
 * MetaInt16, MetaInt32, and MetaInt64.
 */
class PCL_CLASS MetaInt64 : public MetaSignedInteger
{
public:

   /*!
    * Constructs a metaparameter representing a 64-bit signed integer parameter
    * of the specified process class \a *P.
    */
   MetaInt64( MetaProcess* p ) : MetaSignedInteger( p )
   {
   }

   /*!
    * Constructs a metaparameter representing a 64-bit signed integer value in
    * a column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaInt64( MetaTable* t ) : MetaSignedInteger( t )
   {
   }

   /*!
    * Destroys a %MetaInt64 object.
    */
   virtual ~MetaInt64() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaReal
 * \brief A formal description of a real process parameter.
 *
 * Real process parameters can be 32 and 64-bit IEEE 754 floating point
 * values. This leads to the MetaFloat and MetaDouble real process parameter
 * classes, respectively.
 */
class PCL_CLASS MetaReal : public MetaNumeric
{
public:

   /*!
    * Constructs a metaparameter representing a real parameter of the
    * specified process class \a *P.
    */
   MetaReal( MetaProcess* P ) : MetaNumeric( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a real value in a column of a
    * table process parameter represented by the metatable \a *T. The new
    * metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaReal( MetaTable* T ) : MetaNumeric( T )
   {
   }

   /*!
    * Destroys a %MetaReal object.
    */
   virtual ~MetaReal() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsReal() const
   {
      return true;
   }

   /*!
    * Returns the number of decimal digits that should be used to represent
    * actual values of the real process parameter represented by this
    * metaparameter.
    *
    * The returned number of decimal digits will be used for literal
    * representations of the represented parameter, especially in generated
    * scripts. Good examples are automatically generated scripts on the
    * Processing History and ProcessContainer windows in the PixInsight core
    * application.
    *
    * The returned value should be in the range from -1 to 15. A maximum of 7
    * decimal digits will be used for 32-bit real process parameters, and a
    * maximum of 15 decimal digits for 64-bit real process parameters, even
    * if this function returns a larger value.
    *
    * If a value >= 0 is returned, real literal representations are always
    * rounded to the nearest value with the specified amount of decimal digits.
    * If this function returns zero, output values corresponding to this
    * parameter will always be rounded to the nearest integer.
    *
    * If this function returns a negative integer, literal representations will
    * use the standard printf's \e g-format. In this format, numerical values
    * are represented in a compact fashion, with automatic selection of
    * exponential or fixed representations, depending on the value's magnitude
    * and fractional accuracy.
    *
    * \note    The default implementation of this function returns -1. This
    * means that printf's \e g-format is used by default.
    */
   virtual int Precision() const
   {
      return -1; // printf's g format
   }

   /*!
    * Returns true iff actual values of this real process parameter must be
    * represented using scientific notation (printf's \e e-format).
    *
    * If this function returns false, actual parameter values of this parameter
    * will be represented using ordinary decimal notation (printf's
    * \e f-format), unless the Precision() member function returns -1, in which
    * case the automatic \e g-format will always be used.
    *
    * \note    The default implementation of this function returns false. This
    * means that the ordinary decimal notation (printf's \e f-format) is used
    * to represent real parameters by default.
    */
   virtual bool ScientificNotation() const
   {
      return false;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual void PerformTypeAPIDefinitions() const;
   virtual uint32 APIParType() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaFloat
 * \brief A formal description of a 32-bit IEEE 754 floating point process
 *        parameter.
 *
 * Real process parameters can be 32 and 64-bit IEEE 754 floating point values.
 * This leads to the %MetaFloat and MetaDouble real process parameter classes,
 * respectively.
 */
class PCL_CLASS MetaFloat : public MetaReal
{
public:

   /*!
    * Constructs a metaparameter representing a 32-bit floating point parameter
    * of the specified process class \a *P.
    */
   MetaFloat( MetaProcess* P ) : MetaReal( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a 32-bit floating point value in
    * a column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaFloat( MetaTable* T ) : MetaReal( T )
   {
   }

   /*!
    * Destroys a %MetaFloat object.
    */
   virtual ~MetaFloat() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaDouble
 * \brief A formal description of a 64-bit IEEE 754 floating point process
 *        parameter.
 *
 * Real process parameters can be 32 and 64-bit IEEE 754 floating point values.
 * This leads to the MetaFloat and %MetaDouble real process parameter classes,
 * respectively.
 */
class PCL_CLASS MetaDouble : public MetaReal
{
public:

   /*!
    * Constructs a metaparameter representing a 64-bit floating point parameter
    * of the specified process class \a *P.
    */
   MetaDouble( MetaProcess* P ) : MetaReal( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a 64-bit floating point value in
    * a column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaDouble( MetaTable* T ) : MetaReal( T )
   {
   }

   /*!
    * Destroys a %MetaDouble object.
    */
   virtual ~MetaDouble() noexcept( false )
   {
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaBoolean
 * \brief A formal description of a Boolean process parameter.
 *
 * %MetaBoolean represents a Boolean process parameter. A Boolean process
 * parameter can only have one of the two logical values \c true and \c false.
 *
 * \note \b Important - Boolean process parameters must be implemented as
 * 32-bit signed integers (int32). When the PixInsight core application reads
 * or writes a Boolean process parameter, what it reads or writes is actually a
 * 32-bit integer that will represent the Boolean \c true value as a nonzero
 * integer value, and \c false as a zero integer value. The best way to
 * implement Boolean process parameters is by using the pcl_bool class.
 *
 * \sa pcl_bool
 */
class PCL_CLASS MetaBoolean : public MetaParameter
{
public:

   /*!
    * Constructs a metaparameter representing a Boolean parameter of the
    * specified process class \a *P.
    */
   MetaBoolean( MetaProcess* P ) : MetaParameter( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a Boolean value in a column of a
    * table process parameter represented by the metatable \a *T. The new
    * metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaBoolean( MetaTable* T ) : MetaParameter( T )
   {
   }

   /*!
    * Destroys a %MetaBoolean object.
    */
   virtual ~MetaBoolean() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsBoolean() const
   {
      return true;
   }

   /*!
    * Returns the default value for actual Boolean parameters represented by
    * this metaparameter.
    *
    * \note The default implementation of this function returns \c false.
    */
   virtual bool DefaultValue() const
   {
      return false;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

private:

   virtual void PerformTypeAPIDefinitions() const;
   virtual uint32 APIParType() const;
};

/*!
 * \defgroup safe_parameter_types Safe Process Parameter Types
 */

/*!
 * \class pcl_bool
 * \brief A first-class data type that can be safely used to implement Boolean
 *        process parameters on all supported platforms.
 *
 * %pcl_bool is the \e only recommended way to implement process parameters
 * that are formally described by the MetaBoolean class. %Process parameters
 * declared as %pcl_bool instances will have the correct size and numerical
 * type to be accessed and interpreted as valid Boolean parameters by the
 * PixInsight core application on all supported platforms and C++ compilers.
 *
 * \ingroup safe_parameter_types
 * \sa MetaBoolean
 */
class PCL_CLASS pcl_bool
{
public:

   /*!
    * Default constructor. Constructs a %pcl_bool instance with the logical
    * false value.
    */
   pcl_bool() = default;

   /*!
    * Copy constructor. Constructs a %pcl_bool instance whose logical state is
    * true if and only if the specified \a value is nonzero.
    * The T template argument type must have integer equality comparison
    * semantics.
    */
   template <typename T> pcl_bool( const T& value ) : m_value( value ? 1 : 0 )
   {
   }

   /*!
    * Assignment operator. Sets the logical state of this %pcl_bool instance to
    * true if and only if the specified \a value is nonzero.
    * The T template argument type must have integer equality comparison
    * semantics.
    */
   template <typename T> pcl_bool& operator =( const T& value )
   {
      m_value = value ? 1 : 0; return *this;
   }

   /*!
    * bool type conversion operator. Returns the current logical state of this
    * %pcl_bool instance as a bool object.
    */
   operator bool() const
   {
      return m_value != 0;
   }

#ifndef _MSC_VER

   /*!
    * int type conversion operator. Returns zero if the current logical state
    * of this %pcl_bool object is false; one if it is true.
    */
   operator int() const
   {
      return int( m_value );
   }

#endif // !_MSC_VER

   bool operator ==( const pcl_bool& b ) const
   {
      return m_value == b.m_value;
   }

   bool operator <( const pcl_bool& b ) const
   {
      return m_value < b.m_value;
   }

private:

   int32 m_value = 0; // MetaBoolean equivalent type
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaEnumeration
 * \brief A formal description of an enumerated process parameter.
 *
 * %MetaEnumeration represents an enumerated process parameter. An enumerated
 * process parameter defines a finite set of unique-identifier/value
 * associations.
 *
 * \note \b Important - Enumerated parameters must be implemented as 32-bit
 * signed integers (int32). When the PixInsight core application reads or
 * writes an enumerated parameter value, what it actually reads or writes is an
 * enumeration \e value as a 32-bit signed integer, \e not the index of an
 * enumeration element (which should be an unsigned integer). The best way to
 * implement enumerated process parameters is by using the pcl_enum type.
 *
 * \sa pcl_enum
 */
class PCL_CLASS MetaEnumeration : public MetaParameter
{
public:

   /*!
    * Constructs a metaparameter representing an enumerated parameter of the
    * specified process class \a *P.
    */
   MetaEnumeration( MetaProcess* P ) : MetaParameter( P )
   {
   }

   /*!
    * Constructs a metaparameter representing an enumerated value in a column
    * of a table process parameter represented by the metatable \a *T. The new
    * metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaEnumeration( MetaTable* T ) : MetaParameter( T )
   {
   }

   /*!
    * Destroys a %MetaEnumeration object.
    */
   virtual ~MetaEnumeration() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsEnumeration() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns the number of enumeration elements defined in the enumerated
    * process parameter represented by this metaparameter.
    *
    * Each enumeration element is a unique-identifier/value association.
    *
    * The returned value must be > 0.
    */
   virtual size_type NumberOfElements() const = 0;

   /*!
    * Returns the unique enumeration identifier corresponding to the specified
    * enumeration index \a idx, in the enumerated process parameter represented
    * by this metaparameter.
    *
    * Enumeration identifiers must be valid C identifiers, unique within the
    * set of enumeration elements defined in the enumerated process parameter.
    *
    * \note \b Important - An enumeration index should not be confused with an
    * enumeration \e value. Enumeration indices range from 0 to \a n-1, where
    * \a n is the number of enumeration elements defined in the enumerated
    * process parameter.
    */
   virtual IsoString ElementId( size_type idx ) const = 0;

   /*!
    * Returns the enumeration value corresponding to the specified enumeration
    * index \a idx, in the enumerated process parameter represented by this
    * metaparameter.
    *
    * \note \b Important - An enumeration index should not be confused with an
    * enumeration \e value. Enumeration indices range from 0 to \a n-1, where
    * \a n is the number of enumeration elements defined in the enumerated
    * process parameter.
    */
   virtual int ElementValue( size_type idx ) const = 0;

   /*!
    * Returns the enumeration index corresponding to the default value for the
    * enumerated process parameter that this metaparameter represents.
    *
    * The returned value must be in the range 0 to \a n-1, where \a n is the
    * number of enumeration elements defined in the enumerated process
    * parameter.
    *
    * \note The default implementation of this function returns zero.
    */
   virtual size_type DefaultValueIndex() const
   {
      return 0;
   }

   /*!
    * Returns a list of alias identifiers for this enumerated parameter.
    *
    * An enumerated process parameter can define one or more <em>aliased
    * elements</em>. Aliased enumeration elements are useful to maintain
    * compatibility with previous versions of a process.
    *
    * The returned string is a comma-separated list of "alias_id=element_id"
    * items. Formally:
    *
    * <pre>
    * aliased-elements-list:
    *    alias-specification[, aliased-elements-list]
    * alias-specification:
    *    alias-element-id = element-id
    * </pre>
    *
    * When the PixInsight core application imports a process instance (e.g.,
    * from a process icon) it automatically replaces alias enumeration
    * identifiers with actual (current) identifiers. This allows a developer to
    * change the enumeration identifiers of a process parameters without
    * breaking compatibility with existing process instances.
    *
    * \note The default implementation of this member function returns an empty
    * string (no aliases).
    */
   virtual IsoString ElementAliases() const
   {
      return IsoString();
   }

private:

   virtual void PerformTypeAPIDefinitions() const;
   virtual uint32 APIParType() const;
};

/*!
 * \brief An integer type that can be safely used to implement enumerated
 *        process parameters on all supported platforms.
 *
 * %pcl_enum is the \e only recommended way to implement process parameters
 * that are formally described by the MetaEnumeration class. %Process
 * parameters declared as %pcl_enum instances will have the correct size and
 * numerical type to be accessed and interpreted as valid enumeration values by
 * the PixInsight core application on all supported platforms.
 *
 * \ingroup safe_parameter_types
 * \sa MetaEnumeration
 */
typedef int32  pcl_enum;

// ----------------------------------------------------------------------------

/*!
 * \class MetaVariableLengthParameter
 * \brief A formal description of a variable length process parameter.
 *
 * Variable length process parameters can be strings (MetaString), tables
 * (MetaTable) or blocks (MetaBlock).
 */
class PCL_CLASS MetaVariableLengthParameter : public MetaParameter
{
public:

   /*!
    * Constructs a metaparameter representing a variable length parameter of
    * the specified process class \a *P.
    */
   MetaVariableLengthParameter( MetaProcess* P ) : MetaParameter( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a variable-length data item in a
    * column of a table process parameter represented by the metatable \a *T.
    * The new metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaVariableLengthParameter( MetaTable* T ) : MetaParameter( T )
   {
   }

   /*!
    * Destroys a %MetaVariableLengthParameter object.
    */
   virtual ~MetaVariableLengthParameter() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsVariableLength() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns the minimum valid length for the variable length process
    * parameter represented by this metaparameter.
    *
    * \note    The default implementation of this function returns zero, so
    * variable length process parameters can be empty by default.
    */
   virtual size_type MinLength() const
   {
      return 0; // can be empty
   }

   /*!
    * Returns the maximum valid length for the variable length process
    * parameter represented by this metaparameter.
    *
    * If the returned value is zero, then the represented variable length
    * process parameter can have unlimited length.
    *
    * \note    The default implementation of this function returns zero, so
    * variable length process parameters have unlimited length by default.
    */
   virtual size_type MaxLength() const
   {
      return 0; // unlimited length
   }

private:

   virtual void PerformTypeAPIDefinitions() const = 0;
   virtual uint32 APIParType() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaString
 * \brief A formal description of a string process parameter.
 *
 * %String process parameters are sequences of zero-terminated 16-bit Unicode
 * characters (UTF-16).
 */
class PCL_CLASS MetaString : public MetaVariableLengthParameter
{
public:

   /*!
    * Constructs a metaparameter representing a string parameter of the
    * specified process class \a *P.
    */
   MetaString( MetaProcess* P ) : MetaVariableLengthParameter( P )
   {
   }

   /*!
    * Constructs a metaparameter representing a string in a column of a table
    * process parameter represented by the metatable \a *T. The new
    * metaparameter is appended to the list of existing columns in the
    * metatable \a *T.
    */
   MetaString( MetaTable* T ) : MetaVariableLengthParameter( T )
   {
   }

   /*!
    * Destroys a %MetaString object.
    */
   virtual ~MetaString() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsString() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns the default value of the string process parameter represented by
    * this metaparameter.
    *
    * If this function returns zero, the default value for the string process
    * parameter will be an empty string.
    *
    * \note    The default implementation of this function returns zero, so
    * string process parameters have empty string default values.
    */
   virtual String DefaultValue() const
   {
      return String();
   }

   /*!
    * Returns a string with the set of valid characters for the string process
    * parameter represented by this metaparameter.
    *
    * If this function returns zero, any character is valid for the represented
    * string process parameter.
    *
    * \note    The default implementation of this function returns zero, so
    * string process parameters can contain any character by default.
    */
   virtual String AllowedCharacters() const
   {
      return String(); // any character is valid
   }

private:

   virtual void PerformTypeAPIDefinitions() const;
   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaTable
 * \brief A formal description of a table process parameter.
 *
 * A table process parameter is a container that groups a set of process
 * parameters defining the columns of a two-dimensional matrix of parameter
 * values.
 *
 * \note Important: Table process parameters cannot be nested, that is, a table
 * process parameter cannot be specified as a column of an existing table
 * process parameter.
 */
class PCL_CLASS MetaTable : public MetaVariableLengthParameter
{
public:

   /*!
    * Constructs a metaparameter representing a table parameter of the
    * specified process class \a *P.
    */
   MetaTable( MetaProcess* P ) : MetaVariableLengthParameter( P )
   {
   }

   /*!
    * Destroys a %MetaTable object.
    */
   virtual ~MetaTable() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsTable() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

   /*!
    * Returns the address of a metaparameter representing the process
    * parameter at column index \a i.
    *
    * You should not need to call this function directly under normal
    * conditions.
    */
   const MetaParameter* operator[]( size_type i ) const;

private:

   // Nested tables are not allowed.
   MetaTable( MetaTable* );

   virtual void PerformTypeAPIDefinitions() const;
   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class MetaBlock
 * \brief A formal description of a block process parameter.
 *
 * A block process parameter is a container that stores arbitrary data as a
 * sequence of bytes.
 *
 * Block parameters should only be used for parameters that definitely cannot
 * be represented as any of the supported numerical, Boolean, enumerated,
 * or string parameter types. This is because block parameters lead to
 * poorly readable transcriptions in scripts and other visual representations.
 */
class PCL_CLASS MetaBlock : public MetaVariableLengthParameter
{
public:

   /*!
    * Constructs a metaparameter representing a block parameter of the
    * specified process class \a *p.
    */
   MetaBlock( MetaProcess* p ) : MetaVariableLengthParameter( p )
   {
   }

   /*!
    * Constructs a metaparameter representing a data block in a column of a
    * table process parameter represented by the metatable \a *t. The new
    * metaparameter is appended to the list of existing columns in the
    * metatable \a *t.
    */
   MetaBlock( MetaTable* t ) : MetaVariableLengthParameter( t )
   {
   }

   /*!
    * Destroys a %MetaBlock object.
    */
   virtual ~MetaBlock() noexcept( false )
   {
   }

   /*!
    */
   virtual bool IsBlock() const
   {
      return true;
   }

   /*!
    */
   virtual IsoString Id() const = 0;

   /*!
    * %MetaBlock data interpretation modes used for automatic script
    * generation.
    *
    * Supported data interpretation modes:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>MetaBlock::DataAsUInt8</td>    <td>Block data are interpreted as a sequence of 8-bit unsigned integers</td></tr>
    * <tr><td>MetaBlock::DataAsInt8</td>     <td>Block data are interpreted as a sequence of 8-bit signed integers</td></tr>
    * <tr><td>MetaBlock::DataAsUInt16</td>   <td>Block data are interpreted as a sequence of 16-bit unsigned integers</td></tr>
    * <tr><td>MetaBlock::DataAsInt16</td>    <td>Block data are interpreted as a sequence of 16-bit signed integers</td></tr>
    * <tr><td>MetaBlock::DataAsUInt32</td>   <td>Block data are interpreted as a sequence of 32-bit unsigned integers</td></tr>
    * <tr><td>MetaBlock::DataAsInt32</td>    <td>Block data are interpreted as a sequence of 32-bit signed integers</td></tr>
    * <tr><td>MetaBlock::DataAsUInt64</td>   <td>Block data are interpreted as a sequence of 64-bit unsigned integers</td></tr>
    * <tr><td>MetaBlock::DataAsInt64</td>    <td>Block data are interpreted as a sequence of 64-bit signed integers</td></tr>
    * <tr><td>MetaBlock::DataAsFloat</td>    <td>Block data are interpreted as a sequence of float (32-bit IEEE 754 floating point)</td></tr>
    * <tr><td>MetaBlock::DataAsDouble</td>   <td>Block data are interpreted as a sequence of double (64-bit IEEE 754 floating point)</td></tr>
    * </table>
    *
    * The PixInsight core application takes into account these modes to
    * generate readable and meaningful scripts for %MetaBlock parameters.
    */
   enum data_interpretation
   {
      DataAsUInt8,   // 8-bit unsigned integers
      DataAsInt8,    // 8-bit signed integers
      DataAsUInt16,  // 16-bit unsigned integers
      DataAsInt16,   // 16-bit signed integers
      DataAsUInt32,  // 32-bit unsigned integers
      DataAsInt32,   // 32-bit signed integers
      DataAsUInt64,  // 64-bit unsigned integers
      DataAsInt64,   // 64-bit signed integers
      DataAsFloat,   // float (32-bit IEEE 754 floating point)
      DataAsDouble   // double (64-bit IEEE 754 floating point)
   };

   /*!
    * Defines how the data provided by this block parameter will be interpreted
    * by the core application for automatic script generation.
    *
    * The default mode is MetaBlock::DataAsUInt8, which means that the data are
    * interpreted as a sequence of plain bytes by default.
    *
    * This function must be reimplemented to indicate the correct data type for
    * each particular MetaBlock parameter. This is essential to ensure
    * generation of meaningful and readable scripts.
    *
    * \sa MetaBlock::data_interpretation
    */
   virtual data_interpretation DataInterpretation() const
   {
      return DataAsUInt8;
   }

private:

   virtual void PerformTypeAPIDefinitions() const;
   virtual uint32 APIParType() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_MetaParameter_h

// ----------------------------------------------------------------------------
// EOF pcl/MetaParameter.h - Released 2017-07-09T18:07:07Z
