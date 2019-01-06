//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0232
// ----------------------------------------------------------------------------
// ModuleParameterRegistration.h - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

#ifndef __ModuleParameterRegistration_h
#define __ModuleParameterRegistration_h

#define REGISTER_MODULE_PARAMETER(PARAMETER) \
   PARAMETER* The##PARAMETER##Parameter = nullptr

#define DEFINE_INT32_PARAMETER_CLASS(PARAMETER,PARAMETER_NAME,DEFAULT,MIN,MAX)\
PARAMETER::PARAMETER( MetaProcess* P ) : MetaInt32( P )\
{\
   The##PARAMETER##Parameter  = this;\
}\
IsoString PARAMETER::Id() const\
{\
   return PARAMETER_NAME;\
}\
double PARAMETER::DefaultValue() const\
{\
   return DEFAULT;\
}\
double PARAMETER::MinimumValue() const\
{\
   return MIN;\
}\
double PARAMETER::MaximumValue() const\
{\
   return MAX;\
}

#define DEFINE_UINT32_PARAMETER_CLASS(PARAMETER,PARAMETER_NAME,DEFAULT,MIN,MAX)\
PARAMETER::PARAMETER( MetaProcess* P ) : MetaUInt32( P )\
{\
   The##PARAMETER##Parameter  = this;\
}\
IsoString PARAMETER::Id() const\
{\
   return PARAMETER_NAME;\
}\
double PARAMETER::DefaultValue() const\
{\
   return DEFAULT;\
}\
double PARAMETER::MinimumValue() const\
{\
   return MIN;\
}\
double PARAMETER::MaximumValue() const\
{\
   return MAX;\
}

#define DEFINE_STRING_PARAMETER_CLASS(PARAMETER,PARAMETER_NAME)\
PARAMETER::PARAMETER (MetaProcess* P) : MetaString( P )\
{\
   The##PARAMETER##Parameter = this;\
}\
IsoString PARAMETER::Id() const\
{\
   return PARAMETER_NAME;\
}

#define DEFINE_DOUBLE_PARAMETER_CLASS(PARAMETER,PARAMETER_NAME,PRECISION,DEFAULT,MIN,MAX)\
PARAMETER::PARAMETER( MetaProcess* P ) : MetaDouble( P )\
{\
   The##PARAMETER##Parameter = this;\
}\
IsoString PARAMETER::Id() const\
{\
   return PARAMETER_NAME;\
}\
int PARAMETER::Precision() const\
{\
   return PRECISION;\
}\
double PARAMETER::DefaultValue() const\
{\
   return DEFAULT;\
}\
double PARAMETER::MinimumValue() const\
{\
   return MIN;\
}\
double PARAMETER::MaximumValue() const\
{\
   return MAX;\
}

#define DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS(PARAMETER,PARAMETER_NAME,PRECISION,DEFAULT,MIN,MAX)\
PARAMETER::PARAMETER( MetaProcess* P ) : MetaDouble( P )\
{\
   The##PARAMETER##Parameter = this;\
}\
IsoString PARAMETER::Id() const\
{\
   return PARAMETER_NAME;\
}\
int PARAMETER::Precision() const\
{\
   return PRECISION;\
}\
double PARAMETER::DefaultValue() const\
{\
   return DEFAULT;\
}\
double PARAMETER::MinimumValue() const\
{\
   return MIN;\
}\
double PARAMETER::MaximumValue() const\
{\
   return MAX;\
}\
bool PARAMETER::IsReadOnly() const\
{\
   return true;\
}

#define DEFINE_BOOLEAN_PARAMETER_CLASS(PARAMETER,PARAMETER_NAME,DEFAULT)\
PARAMETER::PARAMETER( MetaProcess* P ) : MetaBoolean( P )\
{\
   The##PARAMETER##Parameter = this;\
}\
IsoString PARAMETER::Id() const\
{\
   return PARAMETER_NAME;\
}\
bool PARAMETER::DefaultValue() const\
{\
   return DEFAULT;\
}

#define DEFINE_ENUM_PARAMETER_CLASS(PARAMETER,PARAMETER_NAME,ENUM_LIST,DEFAULT_ITEM)\
PARAMETER::PARAMETER( MetaProcess* P ) : MetaEnumeration( P )\
{\
   The##PARAMETER##Parameter = this;\
}\
IsoString PARAMETER::ElementId( size_type i ) const\
{\
   switch ( i )\
   {\
   ENUM_LIST\
   default:\
   return PARAMETER_NAME "_" #DEFAULT_ITEM;\
   }\
}\
IsoString PARAMETER::Id() const\
{\
   return PARAMETER_NAME;\
}\
size_type PARAMETER::NumberOfElements() const\
{\
   return NumberOfEnumElements;\
}\
int PARAMETER::ElementValue( size_type i ) const\
{\
   return int( i );\
}\
size_type PARAMETER::DefaultValueIndex() const\
{\
   return size_type( Default );\
}

// ============================================================================

#define DECLARE_INT32_PARAMETER_CLASS(PARAMETER)\
class PARAMETER : public MetaInt32\
{\
public:\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
   virtual double DefaultValue() const;\
   virtual double MinimumValue() const;\
   virtual double MaximumValue() const;\
};\
extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_UINT32_PARAMETER_CLASS(PARAMETER)\
class PARAMETER : public MetaUInt32\
{\
public:\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
   virtual double DefaultValue() const;\
   virtual double MinimumValue() const;\
   virtual double MaximumValue() const;\
};\
extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_STRING_PARAMETER_CLASS(PARAMETER)\
class PARAMETER : public MetaString\
{\
public:\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
};\
extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_DOUBLE_PARAMETER_CLASS(PARAMETER)\
class PARAMETER : public MetaDouble\
{\
public:\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
   virtual int Precision() const;\
   virtual double DefaultValue() const;\
   virtual double MinimumValue() const;\
   virtual double MaximumValue() const;\
};\
extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS(PARAMETER)\
class PARAMETER : public MetaDouble\
{\
public:\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
   virtual int Precision() const;\
   virtual double DefaultValue() const;\
   virtual double MinimumValue() const;\
   virtual double MaximumValue() const;\
   virtual bool IsReadOnly() const;\
};\
extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_BOOLEAN_PARAMETER_CLASS(PARAMETER)\
class PARAMETER : public MetaBoolean\
{\
public:\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
   virtual bool DefaultValue() const;\
};\
extern PARAMETER* The##PARAMETER##Parameter

#define ENUM_ITEM(X) X,

# define DECLARE_ENUM_PARAMETER(PARAMETER,ENUM_LIST,DEFAULT_ITEM)\
class PARAMETER : public MetaEnumeration\
{\
public:\
   enum\
   {\
   ENUM_LIST\
   NumberOfEnumElements,\
      Default = DEFAULT_ITEM\
   };\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
   virtual size_type NumberOfElements() const;\
   virtual IsoString ElementId( size_type ) const;\
   virtual int ElementValue( size_type ) const;\
   virtual size_type DefaultValueIndex() const;\
};\
extern PARAMETER* The##PARAMETER##Parameter

# define DECLARE_ENUM_READ_ONLY_PARAMETER(PARAMETER,ENUM_LIST,DEFAULT_ITEM)\
class PARAMETER : public MetaEnumeration\
{\
public:\
   enum\
   {\
   ENUM_LIST\
   NumberOfEnumElements,\
      Default = DEFAULT_ITEM\
   };\
   PARAMETER( MetaProcess* );\
   virtual IsoString Id() const;\
   virtual size_type NumberOfElements() const;\
   virtual IsoString ElementId( size_type ) const;\
   virtual int ElementValue( size_type ) const;\
   virtual size_type DefaultValueIndex() const;\
   virtual bool IsReadOnly() const {return true;}\
};\
extern PARAMETER* The##PARAMETER##Parameter

#endif   // __ModuleParameterRegistration_h

// ----------------------------------------------------------------------------
// EOF ModuleParameterRegistration.h - Released 2018-12-12T09:25:25Z
