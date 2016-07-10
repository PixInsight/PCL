//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.14.0193
// ----------------------------------------------------------------------------
// Alignment.h - Released 2016/06/17 12:50:37 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#ifndef __Alignment_h
#define __Alignment_h

#include <pcl/Matrix.h>
#include <pcl/Vector.h>
#include <pcl/Array.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

enum PierSide {
	West,
	East
};

struct SyncDataPoint {
	double    localSiderialTime;
	double    celestialRA; // true position of the telescope on the sky
	double    celestialDEC;
	double    telecopeRA;  //
	double    telecopeDEC;
	pcl_enum  pierSide;
};

/*
 * Alignment interface
 */
class AlignmentModel {

public:

	AlignmentModel()
	{
	}

	virtual ~AlignmentModel()
	{
	}

	virtual void Apply(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) = 0;

	virtual void fitModel(const Array<SyncDataPoint>& syncPointArray) = 0;

};

// ----------------------------------------------------------------------------


/*
 * Analytical telescope pointing model.
 *
 * Model to capture the effects of
 * - zero point offsets in declination and hour angle
 * - ...
 *
 * http://www.boulder.swri.edu/~buie/idl/downloads/pointing/pointing.pdf
 *
 *
 */
 class LowellPointingModel : public AlignmentModel {
 public:

	LowellPointingModel() : AlignmentModel (), m_modelCoefficientsWest(4), m_modelCoefficientsEast(4), m_modelHACoefficientsWest(4), m_modelHACoefficientsEast(4)
 	{
 	}
	~LowellPointingModel()
	{

	}

	virtual void Apply(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) ;

	virtual void fitModel(const Array<SyncDataPoint>& syncPointArray);

	static AlignmentModel* create(){
		return new LowellPointingModel();
	}
private:

	Vector m_modelCoefficientsWest;
	Vector m_modelCoefficientsEast;
	Vector m_modelHACoefficientsWest;
	Vector m_modelHACoefficientsEast;
};


} // pcl

#endif   // __Alignment_h

// ----------------------------------------------------------------------------
// EOF Alignment.h - Released 2016/06/17 12:50:37 UTC
