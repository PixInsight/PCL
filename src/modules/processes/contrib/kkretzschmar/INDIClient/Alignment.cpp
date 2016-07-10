//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.14.0193
// ----------------------------------------------------------------------------
// Alignment.cpp - Released 2016/06/17 12:50:37 UTC
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

#include "Alignment.h"
#include "INDIMountParameters.h"

#include <pcl/Constants.h>
#include <pcl/Math.h>
#include <pcl/Algebra.h>
#include <pcl/Console.h>
namespace pcl
{

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

void dumpMatrix(const Matrix & matrix){
	Console().WriteLn("Matrix:");
	for (int i = 0; i < matrix.Rows(); ++i) {
		for (int j = 0; j < matrix.Columns(); ++j) {
			Console().Write(String().Format("m[%d,%d]=%6.3f, ", i,j,matrix[i][j]));
		}
		Console().WriteLn();
	}
}

void dumpVector(const Vector & vector){
	Console().WriteLn("Vector:");
	for (int j = 0; j < vector.Length(); ++j) {
		Console().Write(String().Format("v[%d]=%6.3f, ", j, vector[j]));
	}
	Console().WriteLn();
}

void LowellPointingModel::Apply(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) {
	double t_dec = 0;
	double t_ha = 0;
	double siteLatitude = 49.237 * Const<double>::rad();

	if (hourAngle >= 0 ){
		// west
		double hA_rad = hourAngle * 15  * Const<double>::rad();
		t_dec = m_modelCoefficientsWest[0] + m_modelCoefficientsWest[1] * Cos(hA_rad) + m_modelCoefficientsWest[2] * Sin(hA_rad) +
				m_modelCoefficientsWest[3] * (Sin(siteLatitude) * Cos(dec * Const<double>::rad()) - Cos(siteLatitude) * Sin(dec * Const<double>::rad()) * Cos(hA_rad));

		double H = 0;// (m_modelCoefficientsWest[1] * Sin(hA_rad) - m_modelCoefficientsWest[2] *  Cos(hA_rad)) * Tan(dec * Const<double>::rad()) - m_modelCoefficientsWest[3] * Cos(siteLatitude) / Cos(dec * Const<double>::rad()) * Sin(hA_rad);
		t_ha  = m_modelHACoefficientsWest[0] + m_modelHACoefficientsWest[1] / Cos(dec * Const<double>::rad()) + m_modelHACoefficientsWest[2] * Tan(dec * Const<double>::rad()) + m_modelHACoefficientsWest[3] *  hA_rad + H;

	} else {
		// east
		double hA_rad = (hourAngle * 15  + 360 )* Const<double>::rad();
		t_dec = m_modelCoefficientsEast[0] + m_modelCoefficientsEast[1] * Cos(hA_rad) + m_modelCoefficientsEast[2] * Sin(hA_rad) +
				m_modelCoefficientsEast[3] * (	Sin(siteLatitude) * Cos(dec * Const<double>::rad()) - Cos(siteLatitude) * Sin(dec * Const<double>::rad()) * Cos(hA_rad));


		double H = 0; //(m_modelCoefficientsEast[1] * Sin(hA_rad) - m_modelCoefficientsEast[2] *  Cos(hA_rad)) * Tan(dec * Const<double>::rad()) - m_modelCoefficientsEast[3] * Cos(siteLatitude) / Cos(dec * Const<double>::rad()) * Sin(hA_rad);
		t_ha  = m_modelHACoefficientsEast[0] + m_modelHACoefficientsEast[1] / Cos(dec * Const<double>::rad()) + m_modelHACoefficientsEast[2] * Tan(dec * Const<double>::rad()) + m_modelHACoefficientsEast[3] *  hA_rad + H;

	}
	// apply correction
	hourAngleCor =   hourAngle - t_ha * Const<double>::deg() / 15;
	decCor       =  ( dec - t_dec) ;
}

void LowellPointingModel::fitModel(const Array<SyncDataPoint>& syncDataPointArray){

	// Count data points for each pier side
	size_t numOfWestPoints=0;
	size_t numOfEastPoints=0;
	for (auto dataPoint : syncDataPointArray) {
		if ( dataPoint.pierSide == IMCPierSide::West){
			numOfWestPoints++;
		} else {
			numOfEastPoints++;
		}
	}

	/*
	 * Model declination correction
	 */
	// create design matrix to compute the pseudoinverse for multivariate least square regression
	Matrix designMatrixWest(numOfWestPoints,4);
	Matrix designMatrixEast(numOfEastPoints,4);

	Vector decDeltaWest(numOfWestPoints);
	Vector decDeltaEast(numOfEastPoints);

	double westCount=0;
	double eastCount=0;
	for (auto dataPoint : syncDataPointArray) {
		double celestialHourAngle = -25; // invalid value

		if ( dataPoint.pierSide == IMCPierSide::West){
			celestialHourAngle = ( (dataPoint.localSiderialTime - dataPoint.celestialRA) * 15) * Const<double>::rad();

		} else {
			celestialHourAngle = ( (dataPoint.localSiderialTime - dataPoint.celestialRA) * 15 + 360 ) * Const<double>::rad();
		}


		double deltaDec       =  dataPoint.celestialDEC - dataPoint.telecopeDEC;

		double siteLatitude = 49.237 * Const<double>::rad();
		double x = Cos(celestialHourAngle);
		double y = Sin(celestialHourAngle);
		double z = Sin(siteLatitude) * Cos(dataPoint.celestialDEC * Const<double>::rad()) - Cos(siteLatitude) * Sin(dataPoint.celestialDEC * Const<double>::rad()) * Cos(celestialHourAngle);

		if ( dataPoint.pierSide == IMCPierSide::West){
			designMatrixWest[westCount][0]=1;
			designMatrixWest[westCount][1]=x;
			designMatrixWest[westCount][2]=y;
			designMatrixWest[westCount][3]=z;

			decDeltaWest[westCount] = deltaDec;

			westCount++;
		} else {
			designMatrixEast[eastCount][0]=1;
			designMatrixEast[eastCount][1]=x;
			designMatrixEast[eastCount][2]=y;
			designMatrixEast[eastCount][3]=z;

			decDeltaEast[eastCount] = deltaDec;

			eastCount++;
		}

	}

	// compute model parameters

	if (eastCount >=4 ){
		SVD svdEast(designMatrixEast);
		Matrix pseudoInverseEast(4,designMatrixEast.Rows());
		Matrix WInverseEast(4,designMatrixEast.Columns());
		for (int j = 0; j < designMatrixEast.Columns(); ++j){
			for (int i = 0; i < 4; ++i){
				if (i == j && svdEast.W[i] != 0){
					WInverseEast[i][j] = 1.0 / svdEast.W[i];
				}
				if (i != j){
					WInverseEast[i][j] = 0;
				}
			}
		}
		pseudoInverseEast = svdEast.V * WInverseEast * svdEast.U.Transpose();
		m_modelCoefficientsEast =  pseudoInverseEast * decDeltaEast;
	}
	if (westCount >=4){
		SVD svdWest(designMatrixWest); // n x m, m, m x m  , n ==  designMatrixWest.Rows() == num of data points
		Matrix pseudoInverseWest(4,designMatrixWest.Rows());
		Matrix WInverseWest(4,designMatrixWest.Columns());
		for (int j = 0; j < designMatrixWest.Columns(); ++j){
			for (int i = 0; i < 4; ++i) {
				if (i == j && svdWest.W[i] != 0) {
					WInverseWest[i][j] = 1.0 / svdWest.W[i];
				}
				if (i != j){
					WInverseWest[i][j] = 0;
				}
			}
		}


		pseudoInverseWest = svdWest.V * WInverseWest * svdWest.U.Transpose();
		m_modelCoefficientsWest =  pseudoInverseWest * decDeltaWest;
	}

	for (int i=0; i < m_modelCoefficientsEast.Length(); ++i){
		Console().Write(String().Format("a[%d]=%f, ",i,m_modelCoefficientsEast[i]));
		Console().WriteLn();
	}

	for (int i=0; i < m_modelCoefficientsWest.Length(); ++i){
		Console().Write(String().Format("a[%d]=%f, ",i,m_modelCoefficientsWest[i]));
		Console().WriteLn();
	}

	/*
	 * Model hourAngle  correction
	 */

	Matrix haDesignMatrixWest(numOfWestPoints,4);
	Matrix haDesignMatrixEast(numOfEastPoints,4);

	Vector haDeltaWest(numOfWestPoints);
	Vector haDeltaEast(numOfEastPoints);
	westCount=0;
	eastCount=0;
	for (auto dataPoint : syncDataPointArray) {
		double celestialHourAngle = -25; // invalid value
		double telescopeHourAngle = -25; // invalid value
		if ( dataPoint.pierSide == IMCPierSide::West){
			celestialHourAngle = ( (dataPoint.localSiderialTime - dataPoint.celestialRA) * 15) * Const<double>::rad();
			telescopeHourAngle = ( (dataPoint.localSiderialTime - dataPoint.telecopeRA) * 15 ) * Const<double>::rad();

		} else {
			celestialHourAngle = ( (dataPoint.localSiderialTime - dataPoint.celestialRA) * 15 + 360 ) * Const<double>::rad();
			telescopeHourAngle = ( (dataPoint.localSiderialTime - dataPoint.telecopeRA) * 15 + 360 ) * Const<double>::rad();
		}
		double deltaHourAngle     = celestialHourAngle - telescopeHourAngle;
		double siteLatitude       = 49 * Const<double>::rad();
		double x = Cos(celestialHourAngle);
		double y = Sin(celestialHourAngle);
		double S = 1.0 / Cos(dataPoint.celestialDEC * Const<double>::rad());
		double T = Tan(dataPoint.celestialDEC * Const<double>::rad());
		double H = 0;
		if (dataPoint.pierSide == IMCPierSide::West) {
			H = 0;//(m_modelCoefficientsWest[1] * y - m_modelCoefficientsWest[2] * x) * T - m_modelCoefficientsWest[3] * Cos(siteLatitude) * S * y;

			haDesignMatrixWest[westCount][0]=1;
			haDesignMatrixWest[westCount][1]=S;
			haDesignMatrixWest[westCount][2]=T;
			haDesignMatrixWest[westCount][3]=celestialHourAngle;

			haDeltaWest[westCount] = deltaHourAngle - H;
			westCount++;

		} else {
			H = 0;//(m_modelCoefficientsEast[1] * y - m_modelCoefficientsEast[2] * x) * T - m_modelCoefficientsEast[3] * Cos(siteLatitude) * S * y;

			haDesignMatrixEast[eastCount][0]=1;
			haDesignMatrixEast[eastCount][1]=S;
			haDesignMatrixEast[eastCount][2]=T;
			haDesignMatrixEast[eastCount][3]=celestialHourAngle;

			haDeltaEast[eastCount] = deltaHourAngle - H;
			eastCount++;
		}
	}

	dumpMatrix(haDesignMatrixWest);
	dumpVector(haDeltaWest);
	dumpMatrix(haDesignMatrixEast);
	dumpVector(haDeltaEast);

	// compute model parameters

	if (eastCount >=4 ){
		SVD svdEast(haDesignMatrixEast);
		Matrix pseudoInverseEast(4,haDesignMatrixEast.Rows());
		Matrix WInverseEast(4,haDesignMatrixEast.Columns());
		for (int j = 0; j < haDesignMatrixEast.Columns(); ++j){
			for (int i = 0; i < 4; ++i){
				if (i == j && svdEast.W[i] != 0){
					WInverseEast[i][i] = 1.0 / svdEast.W[i];
				}
				if (i != j){
					WInverseEast[i][j] = 0.0;
				}
			}
		}
		pseudoInverseEast = svdEast.V * WInverseEast * svdEast.U.Transpose();
		m_modelHACoefficientsEast =  pseudoInverseEast * haDeltaEast;
	}
	if (westCount >=4){
		SVD svdWest(haDesignMatrixWest);
		Matrix pseudoInverseWest(4,haDesignMatrixWest.Rows());
		Matrix WInverseWest(4,haDesignMatrixWest.Columns());
		for (int j = 0; j < haDesignMatrixWest.Columns(); ++j){
			for (int i = 0; i < 4; ++i) {
				if ( i == j && svdWest.W[i] != 0) {
					WInverseWest[i][i] = 1.0 / svdWest.W[i];
				}
				if (i != j){
					WInverseWest[i][j] = 0.0;
				}
			}
		}
		pseudoInverseWest = svdWest.V * WInverseWest * svdWest.U.Transpose();
		m_modelHACoefficientsWest =  pseudoInverseWest * haDeltaWest;
	}


	for (int i=0; i < m_modelHACoefficientsEast.Length(); ++i){
		Console().Write(String().Format("b[%d]=%f, ",i,m_modelHACoefficientsEast[i]));
		Console().WriteLn();
	}

	for (int i=0; i < m_modelHACoefficientsWest.Length(); ++i){
		Console().Write(String().Format("b[%d]=%f, ",i,m_modelHACoefficientsWest[i]));
		Console().WriteLn();
	}

}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Alignment.cpp - Released 2016/06/17 12:50:37 UTC
