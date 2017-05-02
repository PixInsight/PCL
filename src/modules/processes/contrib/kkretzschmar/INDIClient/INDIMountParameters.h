//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0203
// ----------------------------------------------------------------------------
// INDIMountParameters.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#ifndef __INDIMountParameters_h
#define __INDIMountParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class IMCDeviceName : public MetaString
{
public:

   IMCDeviceName( MetaProcess* );

   virtual IsoString Id() const;
};

extern IMCDeviceName* TheIMCDeviceNameParameter;

// ----------------------------------------------------------------------------

class IMCCommand : public MetaEnumeration
{
public:

   enum
   {
      Unpark,
      Park,
      MoveNorthStart,
      MoveNorthStop,
      MoveSouthStart,
      MoveSouthStop,
      MoveWestStart,
      MoveWestStop,
      MoveEastStart,
      MoveEastStop,
      GoTo,
      Sync,
      ParkDefault,
      NumberOfCommands,
      Default = Unpark
   };

   IMCCommand( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IMCCommand* TheIMCCommandParameter;

// ----------------------------------------------------------------------------

class IMCSlewRate : public MetaEnumeration
{
public:

   enum
   {
      Guide,
      Centering,
      Find,
      Max,
      NumberOfCommands,
      Default = Max
   };

   IMCSlewRate( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IMCSlewRate* TheIMCSlewRateParameter;

// ----------------------------------------------------------------------------

class IMCTargetRA : public MetaDouble
{
public:

   IMCTargetRA( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IMCTargetRA* TheIMCTargetRAParameter;

// ----------------------------------------------------------------------------

class IMCTargetDec : public MetaDouble
{
public:

   IMCTargetDec( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IMCTargetDec* TheIMCTargetDecParameter;

// ----------------------------------------------------------------------------

class IMCComputeApparentPosition : public MetaBoolean
{
public:

   IMCComputeApparentPosition( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IMCComputeApparentPosition* TheIMCComputeApparentPositionParameter;

// ----------------------------------------------------------------------------

class IMCCurrentLST : public MetaDouble
{
public:

   IMCCurrentLST( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual bool IsReadOnly() const;
};

extern IMCCurrentLST* TheIMCCurrentLSTParameter;

// ----------------------------------------------------------------------------

class IMCCurrentRA : public MetaDouble
{
public:

   IMCCurrentRA( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual bool IsReadOnly() const;
};

extern IMCCurrentRA* TheIMCCurrentRAParameter;

// ----------------------------------------------------------------------------

class IMCCurrentDec : public MetaDouble
{
public:

   IMCCurrentDec( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual bool IsReadOnly() const;
};

extern IMCCurrentDec* TheIMCCurrentDecParameter;

// ----------------------------------------------------------------------------

class IMCApparentTargetRA : public MetaDouble
{
public:

   IMCApparentTargetRA( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual bool IsReadOnly() const;
};

extern IMCApparentTargetRA* TheIMCApparentTargetRAParameter;

// ----------------------------------------------------------------------------

class IMCApparentTargetDec : public MetaDouble
{
public:

   IMCApparentTargetDec( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual bool IsReadOnly() const;
};

extern IMCApparentTargetDec* TheIMCApparentTargetDecParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __INDIMountParameters_h

// ----------------------------------------------------------------------------
// EOF INDIMountParameters.h - Released 2017-05-02T09:43:01Z
