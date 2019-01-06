//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0316
// ----------------------------------------------------------------------------
// StarGeneratorParameters.h - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#ifndef __StarGeneratorParameters_h
#define __StarGeneratorParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SGStarDatabasePath : public MetaString
{
public:

   SGStarDatabasePath( MetaProcess* );

   virtual IsoString Id() const;
};

extern SGStarDatabasePath* TheSGStarDatabasePathParameter;

// ----------------------------------------------------------------------------

class SGCenterRA : public MetaDouble
{
public:

   SGCenterRA( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGCenterRA* TheSGCenterRAParameter;

// ----------------------------------------------------------------------------

class SGCenterDec : public MetaDouble
{
public:

   SGCenterDec( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGCenterDec* TheSGCenterDecParameter;

// ----------------------------------------------------------------------------

class SGEpoch : public MetaDouble
{
public:

   SGEpoch( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGEpoch* TheSGEpochParameter;

// ----------------------------------------------------------------------------

class SGProjectionSystem : public MetaEnumeration
{
public:

   enum { Conformal,
          Gnomonic,
          NumberOfProjectionSystems,
          Default = Gnomonic };

   SGProjectionSystem( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern SGProjectionSystem* TheSGProjectionSystemParameter;

// ----------------------------------------------------------------------------

class SGFocalLength : public MetaFloat
{
public:

   SGFocalLength( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGFocalLength* TheSGFocalLengthParameter;

// ----------------------------------------------------------------------------

class SGPixelSize : public MetaFloat
{
public:

   SGPixelSize( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGPixelSize* TheSGPixelSizeParameter;

// ----------------------------------------------------------------------------

class SGSensorWidth : public MetaInt32
{
public:

   SGSensorWidth( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGSensorWidth* TheSGSensorWidthParameter;

// ----------------------------------------------------------------------------

class SGSensorHeight : public MetaInt32
{
public:

   SGSensorHeight( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGSensorHeight* TheSGSensorHeightParameter;

// ----------------------------------------------------------------------------

class SGLimitMagnitude : public MetaFloat
{
public:

   SGLimitMagnitude( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGLimitMagnitude* TheSGLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class SGStarFWHM : public MetaFloat
{
public:

   SGStarFWHM( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGStarFWHM* TheSGStarFWHMParameter;

// ----------------------------------------------------------------------------

class SGNonlinear : public MetaBoolean
{
public:

   SGNonlinear( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SGNonlinear* TheSGNonlinearParameter;

// ----------------------------------------------------------------------------

class SGTargetMinimumValue : public MetaFloat
{
public:

   SGTargetMinimumValue( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SGTargetMinimumValue* TheSGTargetMinimumValueParameter;

// ----------------------------------------------------------------------------

class SGOutputMode : public MetaEnumeration
{
public:

   enum { Image,
          CSVFile,
          NumberOfOutputModes,
          Default = Image };

   SGOutputMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern SGOutputMode* TheSGOutputModeParameter;

// ----------------------------------------------------------------------------

class SGOutputFilePath : public MetaString
{
public:

   SGOutputFilePath( MetaProcess* );

   virtual IsoString Id() const;
};

extern SGOutputFilePath* TheSGOutputFilePathParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __StarGeneratorParameters_h

// ----------------------------------------------------------------------------
// EOF StarGeneratorParameters.h - Released 2018-12-12T09:25:25Z
