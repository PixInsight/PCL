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
			Console().Write(String().Format("m[%d,%d]=%6.6f, ", i,j,matrix[i][j]));
		}
		Console().WriteLn();
	}
}

void dumpVector(const Vector & vector){
	Console().WriteLn("Vector:");
	for (int j = 0; j < vector.Length(); ++j) {
		Console().Write(String().Format("v[%d]=%6.6f, ", j, vector[j]));
	}
	Console().WriteLn();
}

void AlignmentModel::getPseudoInverse(Matrix& pseudoInverse, const Matrix& matrix){
	if (pseudoInverse.Rows() != matrix.Columns() || pseudoInverse.Columns() != matrix.Rows()){
		throw Error( "Internal error: AlignmentModel::getPseudoInverse: Matrix dimensions do not match" );
	}

	SVD svd(matrix);
	dumpVector(svd.W);
	Matrix WInverse(matrix.Columns(),matrix.Columns());
	for (int j = 0; j < matrix.Columns(); ++j){
		for (int i = 0; i < matrix.Columns(); ++i) {
			if (i == j && svd.W[i] != 0) {
				WInverse[i][j] = 1.0 / svd.W[i];
			}
			if (i != j){
				WInverse[i][j] = 0;
			}
		}
	}
	pseudoInverse = svd.V * WInverse * svd.U.Transpose();
}


void LowellPointingModel::Apply(double& hourAngleCor, double& decCor, double hourAngle, double dec) {
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

	dumpVector(m_modelCoefficientsEast);
			dumpVector(m_modelCoefficientsWest);
			dumpVector(m_modelHACoefficientsEast);
			dumpVector(m_modelHACoefficientsWest);
}


void LowellPointingModel::writeObject(const String& fileName)
{
	// save model parameters to disk
	IsoString fileContent;

	for (int i=0; i < m_modelCoefficientsEast.Length(); ++i){
		if (i < m_modelCoefficientsEast.Length()-1)
			fileContent.Append(IsoString().Format("%f,",m_modelCoefficientsEast[i]));
		else
			fileContent.Append(IsoString().Format("%f",m_modelCoefficientsEast[i]));
	}
	fileContent.Append("\n");
	for (int i=0; i < m_modelCoefficientsWest.Length(); ++i){
		if (i < m_modelCoefficientsWest.Length()-1)
			fileContent.Append(IsoString().Format("%f,",m_modelCoefficientsWest[i]));
		else
			fileContent.Append(IsoString().Format("%f",m_modelCoefficientsWest[i]));
	}
	fileContent.Append("\n");
	for (int i=0; i < m_modelHACoefficientsEast.Length(); ++i){
		if (i < m_modelHACoefficientsEast.Length()-1)
			fileContent.Append(IsoString().Format("%f,",m_modelHACoefficientsEast[i]));
		else
			fileContent.Append(IsoString().Format("%f",m_modelHACoefficientsEast[i]));
	}
	fileContent.Append("\n");
	for (int i=0; i < m_modelHACoefficientsWest.Length(); ++i){
		if (i < m_modelHACoefficientsWest.Length()-1)
			fileContent.Append(IsoString().Format("%f,",m_modelHACoefficientsWest[i]));
		else
			fileContent.Append(IsoString().Format("%f",m_modelHACoefficientsWest[i]));
	}
	fileContent.Append("\n");


	if (File::Exists(fileName)){
		IsoStringList syncPointDataList = File::ReadLines(fileName);
		for (auto line : syncPointDataList){
			fileContent.Append(line);
			fileContent.Append("\n");
		}
		File::Remove(fileName);
	}
	File::WriteTextFile(fileName,fileContent);
}

void LowellPointingModel::readObject(const String& fileName)
{
	IsoStringList modeParameterList = File::ReadLines(fileName);

	for (size_t i = 0 ; i < modeParameterList.Length(); ++i) {

		IsoStringList tokens;
		modeParameterList[i].Break(tokens, ",", true);

		switch (i){
		case 0:
			for (int j = 0; j < m_modelCoefficientsEast.Length(); ++j){
				m_modelCoefficientsEast[j] = tokens[j].ToDouble();
			}
		break;
		case 1:
			for (int j = 0; j < m_modelCoefficientsWest.Length(); ++j){
				m_modelCoefficientsWest[j] = tokens[j].ToDouble();
			}
		break;
		case 2:
			for (int j = 0; j < m_modelHACoefficientsEast.Length(); ++j){
				m_modelHACoefficientsEast[j] = tokens[j].ToDouble();
			}
		break;
		case 3:
			for (int j = 0; j < m_modelHACoefficientsWest.Length(); ++j){
				m_modelHACoefficientsWest[j] = tokens[j].ToDouble();
			}
		break;
		}
	}
}


void AnalyticalPointingModel::evaluateBasis(Matrix& basisVectors, double hourAngle, double dec)
{
	if (basisVectors.Rows() != 3 || basisVectors.Columns() != 6)
	{
		throw Error( "Internal error: AnalyticalPointingModel::evaluateBasis: Matrix dimensions do not match" );
	}

	double ctc  = Cos(hourAngle * Const<double>::rad());
	double cdc  = Cos(dec * Const<double>::rad());

	double stc  = Sin(hourAngle * Const<double>::rad());
	double sdc  = Sin(dec * Const<double>::rad());

	// p1
	basisVectors[0][0] = 0;
	basisVectors[1][0] = -sdc;
	basisVectors[2][0] = cdc * stc ;
	// p2
	basisVectors[0][1] = sdc;
	basisVectors[1][1] = 0;
	basisVectors[2][1] = -cdc * ctc ;
	// p3
	basisVectors[0][2] = -cdc * stc;
	basisVectors[1][2] =  cdc * ctc;
	basisVectors[2][2] = 0;
	// p4
	basisVectors[0][3] = sdc * stc;
	basisVectors[1][3] = -sdc * ctc;
	basisVectors[2][3] = 0;
	// p5
	basisVectors[0][4] = sdc * ctc;
	basisVectors[1][4] = sdc * stc;
	basisVectors[2][4] = -cdc ;
	// p6
	basisVectors[0][5] = -stc;
	basisVectors[1][5] =  ctc;
	basisVectors[2][5] = 0;

}

void AnalyticalPointingModel::Apply(double& hourAngleCor, double& decCor, double hourAngle, double dec)
{
	Matrix basisVectors(3,m_numOfModelParameters);

	evaluateBasis(basisVectors,hourAngle*15,dec);

	// compute correction vector
	Vector alignCorrection(3);
	alignCorrection = (*m_modelParameters)[0] * basisVectors.ColumnVector(0);
	for (size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++){
		alignCorrection +=  (*m_modelParameters)[modelIndex] * basisVectors.ColumnVector(modelIndex);
	}

	dumpVector(alignCorrection);
	// compute p0
	Vector p0(3);
	p0[0] = Cos(hourAngle * 15 * Const<double>::rad()) * Cos(dec * Const<double>::rad());
	p0[1] = Sin(hourAngle * 15 * Const<double>::rad()) * Cos(dec * Const<double>::rad());
	p0[2] = Sin(dec * Const<double>::rad());

	dumpVector(p0);
	dumpVector(alignCorrection);
	// compute corrected p (pCorr)
	Vector pCorr(3);
	pCorr = p0 + alignCorrection;

	hourAngleCor  = ArcTan(pCorr[1],pCorr[0]) * Const<double>::deg() / 15;
	decCor = ArcTan(pCorr[2],Sqrt(pCorr[0]*pCorr[0] + pCorr[1] * pCorr[1])) * Const<double>::deg();

}

void AnalyticalPointingModel::fitModel(const Array<SyncDataPoint>& syncPointArray)
{
	//  design matrix
	Matrix designMatrix(3 * syncPointArray.Length(),m_numOfModelParameters);
	// alignmentErrorVecotr
	Vector alignmentError(3 * syncPointArray.Length());

	size_t count=0;
	for (auto syncPoint : syncPointArray) {
		double celestialHourAngle = (syncPoint.localSiderialTime - syncPoint.celestialRA) * 15 ;
		double telescopeHourAngle = (syncPoint.localSiderialTime - syncPoint.telecopeRA)  * 15 ;

		// calculate design matrix
		Matrix basisVectors(3,m_numOfModelParameters);

		evaluateBasis(basisVectors,celestialHourAngle,syncPoint.celestialDEC);

		for (size_t modelIndex = 0; modelIndex < m_numOfModelParameters; modelIndex++){
			designMatrix[3*count]    [modelIndex] = basisVectors[0][modelIndex];
			designMatrix[3*count + 1][modelIndex] = basisVectors[1][modelIndex];
			designMatrix[3*count + 2][modelIndex] = basisVectors[2][modelIndex];
		}

		// calculate alignment error
		double ctc  = Cos(celestialHourAngle * Const<double>::rad());
		double cdc  = Cos(syncPoint.celestialDEC * Const<double>::rad());
		double stc  = Sin(celestialHourAngle * Const<double>::rad());
		double sdc  = Sin(syncPoint.celestialDEC * Const<double>::rad());

		double ctt  = Cos(telescopeHourAngle * Const<double>::rad());
		double cdt  = Cos(syncPoint.telecopeDEC * Const<double>::rad());
		double stt  = Sin(telescopeHourAngle * Const<double>::rad());
		double sdt  = Sin(syncPoint.telecopeDEC * Const<double>::rad());

		alignmentError[3*count] = ctt * cdt - ctc * cdc;
		alignmentError[3*count + 1] = stt * cdt - stc * cdc;
		alignmentError[3*count + 2] = sdt  - sdc;

		count++;
	}

	// compute pseudo inverse
	Matrix pseudoInverse(designMatrix.Columns(),designMatrix.Rows());
	AnalyticalPointingModel::getPseudoInverse(pseudoInverse,designMatrix);

	// fit parameters
	*m_modelParameters = pseudoInverse * alignmentError;
}


void AnalyticalPointingModel::writeObject(const String& fileName)
{
	// save model parameters to disk
	IsoString fileContent;

	for (int i=0; i < this->m_numOfModelParameters; ++i){
		if (i < m_numOfModelParameters-1)
			fileContent.Append(IsoString().Format("%f,",(*m_modelParameters)[i]));
		else
			fileContent.Append(IsoString().Format("%f",(*m_modelParameters)[i]));
	}

	fileContent.Append("\n");


	if (File::Exists(fileName)){
		IsoStringList syncPointDataList = File::ReadLines(fileName);
		for (auto line : syncPointDataList){
			fileContent.Append(line);
			fileContent.Append("\n");
		}
		File::Remove(fileName);
	}
	File::WriteTextFile(fileName,fileContent);
}

void AnalyticalPointingModel::readObject(const String& fileName)
{
	IsoStringList modelParameterList = File::ReadLines(fileName);

	for (size_t i = 0 ; i < modelParameterList.Length(); ++i) {

		IsoStringList tokens;
		modelParameterList[i].Break(tokens, ",", true);

		for (int j = 0; j < m_numOfModelParameters; ++j){
			(*m_modelParameters)[j] = tokens[j].ToDouble();
		}
	}
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Alignment.cpp - Released 2016/06/17 12:50:37 UTC
