//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0347
// ----------------------------------------------------------------------------
// GREYCstorationParameters.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#ifndef __GREYCstorationParameters_h
#define __GREYCstorationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GREYCsAmplitude : public MetaFloat
{
public:

   GREYCsAmplitude( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsAmplitude* TheGREYCsAmplitudeParameter;

// ----------------------------------------------------------------------------

class GREYCsIterations : public MetaInt32
{
public:

   GREYCsIterations( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsIterations* TheGREYCsIterationsParameter;

// ----------------------------------------------------------------------------

class GREYCsSharpness : public MetaFloat
{
public:

   GREYCsSharpness( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsSharpness* TheGREYCsSharpnessParameter;

// ----------------------------------------------------------------------------

class GREYCsAnisotropy : public MetaFloat
{
public:

   GREYCsAnisotropy( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsAnisotropy* TheGREYCsAnisotropyParameter;

// ----------------------------------------------------------------------------

class GREYCsAlpha : public MetaFloat
{
public:

   GREYCsAlpha( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsAlpha* TheGREYCsAlphaParameter;

// ----------------------------------------------------------------------------

class GREYCsSigma : public MetaFloat
{
public:

   GREYCsSigma( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsSigma* TheGREYCsSigmaParameter;

// ----------------------------------------------------------------------------

class GREYCsFastApproximation : public MetaBoolean
{
public:

   GREYCsFastApproximation( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GREYCsFastApproximation* TheGREYCsFastApproximationParameter;

// ----------------------------------------------------------------------------

class GREYCsPrecision : public MetaFloat
{
public:

   GREYCsPrecision( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsPrecision* TheGREYCsPrecisionParameter;

// ----------------------------------------------------------------------------

class GREYCsSpatialStepSize : public MetaFloat
{
public:

   GREYCsSpatialStepSize( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsSpatialStepSize* TheGREYCsSpatialStepSizeParameter;

// ----------------------------------------------------------------------------

class GREYCsAngularStepSize : public MetaFloat
{
public:

   GREYCsAngularStepSize( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GREYCsAngularStepSize* TheGREYCsAngularStepSizeParameter;

// ----------------------------------------------------------------------------

class GREYCsInterpolation : public MetaEnumeration
{
public:

   enum { Nearest,      // Nearest neighbor
          Bilinear,     // Bilinear
          RungeKutta,   // 2nd order Runge-Kutta
          Default = Nearest };

   GREYCsInterpolation( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern GREYCsInterpolation* TheGREYCsInterpolationParameter;

// ----------------------------------------------------------------------------

class GREYCsCoupledChannels : public MetaBoolean
{
public:

   GREYCsCoupledChannels( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GREYCsCoupledChannels* TheGREYCsCoupledChannelsParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __GREYCstorationParameters_h

// ----------------------------------------------------------------------------
// EOF GREYCstorationParameters.h - Released 2019-01-21T12:06:41Z
