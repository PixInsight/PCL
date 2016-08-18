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
	Matrix WInverse(matrix.Columns(),matrix.Columns());
	for (int j = 0; j < matrix.Columns(); ++j){
		for (int i = 0; i < matrix.Columns(); ++i) {
			if (i == j ) {
				if (Abs(svd.W[i]) > 1e-15)
					WInverse[i][j] = 1.0 / svd.W[i];
				else {
					WInverse[i][j] =  0;
				}
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

		t_ha  = m_modelHACoefficientsWest[0] + m_modelHACoefficientsWest[1] / Cos(dec * Const<double>::rad()) + m_modelHACoefficientsWest[2] * Tan(dec * Const<double>::rad()) + m_modelHACoefficientsWest[3] *  hA_rad ;

	} else {
		// east
		double hA_rad = (hourAngle * 15  + 360 )* Const<double>::rad();
		t_dec = m_modelCoefficientsEast[0] + m_modelCoefficientsEast[1] * Cos(hA_rad) + m_modelCoefficientsEast[2] * Sin(hA_rad) +
				m_modelCoefficientsEast[3] * (	Sin(siteLatitude) * Cos(dec * Const<double>::rad()) - Cos(siteLatitude) * Sin(dec * Const<double>::rad()) * Cos(hA_rad));


		t_ha  = m_modelHACoefficientsEast[0] + m_modelHACoefficientsEast[1] / Cos(dec * Const<double>::rad()) + m_modelHACoefficientsEast[2] * Tan(dec * Const<double>::rad()) + m_modelHACoefficientsEast[3] *  hA_rad ;

	}
	// apply correction
	hourAngleCor =   hourAngle - t_ha * Const<double>::deg() / 15;
	decCor       =  ( dec - t_dec) ;
}

void LowellPointingModel::ApplyInverse(double& hourAngleCor, double& decCor, const double hourAngle, const double dec){

}

void LowellPointingModel::fitModel(const Array<SyncDataPoint>& syncDataPointArray, pcl_enum pierSide){

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
		double z = 0;//Sin(siteLatitude) * Cos(dataPoint.celestialDEC * Const<double>::rad()) - Cos(siteLatitude) * Sin(dataPoint.celestialDEC * Const<double>::rad()) * Cos(celestialHourAngle);

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
		Matrix pseudoInverseEast(4,designMatrixEast.Rows());
		getPseudoInverse(pseudoInverseEast,designMatrixEast);

		m_modelCoefficientsEast =  pseudoInverseEast * decDeltaEast;
	}
	if (westCount >=4){
		Matrix pseudoInverseWest(4,designMatrixWest.Rows());
		getPseudoInverse(pseudoInverseWest,designMatrixWest);

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
			haDesignMatrixWest[westCount][3]=0;/*celestialHourAngle;*/

			haDeltaWest[westCount] = deltaHourAngle - H;
			westCount++;

		} else {
			H = 0;//(m_modelCoefficientsEast[1] * y - m_modelCoefficientsEast[2] * x) * T - m_modelCoefficientsEast[3] * Cos(siteLatitude) * S * y;

			haDesignMatrixEast[eastCount][0]=1;
			haDesignMatrixEast[eastCount][1]=S;
			haDesignMatrixEast[eastCount][2]=T;
			haDesignMatrixEast[eastCount][3]=0; /*celestialHourAngle;*/

			haDeltaEast[eastCount] = deltaHourAngle - H;
			eastCount++;
		}
	}

	// compute model parameters
	if (eastCount >=4 ){

		Matrix pseudoInverseEast(4,haDesignMatrixEast.Rows());
		getPseudoInverse(pseudoInverseEast,haDesignMatrixEast);

		m_modelHACoefficientsEast =  pseudoInverseEast * haDeltaEast;
	}
	if (westCount >=4){

		Matrix pseudoInverseWest(4,haDesignMatrixWest.Rows());
		getPseudoInverse(pseudoInverseWest,haDesignMatrixWest);

		m_modelHACoefficientsWest =  pseudoInverseWest * haDeltaWest;
	}

}


void LowellPointingModel::writeObject(const String& fileName, pcl_enum pierSide)
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

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

void TpointPointingModel::evaluateBasis(Matrix& basisVectors, double hourAngle, double dec)
{
	if (basisVectors.Rows() != 2 || basisVectors.Columns() != m_numOfModelParameters)
	{
		throw Error( "Internal error: TpointPointingModel::evaluateBasis: Matrix dimensions do not match" );
	}
	double ctc  = Cos(hourAngle * Const<double>::rad() * 15);
	double cdc  = Cos(dec * Const<double>::rad());

	double stc  = Sin(hourAngle * Const<double>::rad() * 15);
/*	double stcd4  = Sin(hourAngle * Const<double>::rad() * 15 * 0.4);

	double stcd5  = Sin(hourAngle * Const<double>::rad() * 15 * 0.5);
	double stcd6  = Sin(hourAngle * Const<double>::rad() * 15 * 0.6);
	double stc2  = Sin(hourAngle * Const<double>::rad() * 15 * 2);
	double stc4  = Sin(hourAngle * Const<double>::rad() * 15 * 4);
	double stc3  = Sin(hourAngle * Const<double>::rad() * 15 * 3);*/

	double sdc  = Sin(dec * Const<double>::rad());

	double secdc = 1.0 / Cos(dec * Const<double>::rad());
	double tandc = Tan(dec * Const<double>::rad());

	double clat  = Cos(m_siteLatitude);
	double slat  = Sin(m_siteLatitude);


	if (CHECK_BIT(m_modelConfig,1)){
		// zero-point offset in ra readout
		basisVectors[0][0] = 1;
		basisVectors[1][0] = 0;
		// zero-point offset in dec readout
		basisVectors[0][1] = 0;
		basisVectors[1][1] = 1;
	} else{
		basisVectors[0][0] = 0;
		basisVectors[1][0] = 0;
		basisVectors[0][1] = 0;
		basisVectors[1][1] = 0;
	}

	if (CHECK_BIT(m_modelConfig,2)){
		// collimation error
		basisVectors[0][2] = secdc;
		basisVectors[1][2] =  0;

	} else {
		// collimation error
		basisVectors[0][2] =  0;
		basisVectors[1][2] =  0;
	}
	if (CHECK_BIT(m_modelConfig,3)){
		// non-perpendicular dec-ra axis error
		basisVectors[0][3] = tandc;
		basisVectors[1][3] = 0;
	} else {
		// non-perpendicular dec-ra axis error
		basisVectors[0][3] =  0;
		basisVectors[1][3] =  0;

	}
	if (CHECK_BIT(m_modelConfig,4)){
		// polar-axis horizontal displacement
		basisVectors[0][4] = -ctc * tandc;
		basisVectors[1][4] = stc;
	} else {
		// polar-axis horizontal displacement
		basisVectors[0][4] = 0;
		basisVectors[1][4] = 0;
	}
	if (CHECK_BIT(m_modelConfig,5)){
		// polar-axis vertical displacement
		basisVectors[0][5] = stc * tandc;
		basisVectors[1][5] = ctc;
	} else {
		// polar-axis vertical displacement
		basisVectors[0][5] = 0;
		basisVectors[1][5] = 0;
	}
	if (CHECK_BIT(m_modelConfig,6)){
		// tube flexure
		basisVectors[0][6] = clat * stc * secdc;
		basisVectors[1][6] = clat * ctc * sdc - slat * cdc;
	} else {
		basisVectors[0][6] = 0;
		basisVectors[1][6] = 0;
	}
	if (CHECK_BIT(m_modelConfig,7)){
		// fork flexure
		basisVectors[0][7] = 0;
		basisVectors[1][7] = ctc;
	} else {
		basisVectors[0][7] = 0;
		basisVectors[1][7] = 0;
	}

	if (CHECK_BIT(m_modelConfig,8)){
		// delta-axis flexure
		basisVectors[0][8] = 0;//cdc * ctc + slat * tandc;
		basisVectors[1][8] = dec * Const<double>::rad();
	} else {
		basisVectors[0][8] = 0;
		basisVectors[1][8] = 0;

	}

}

void TpointPointingModel::Apply(double& hourAngleCor, double& decCor, double hourAngle, double dec)
{
	Matrix basisVectors(2,m_numOfModelParameters);

	evaluateBasis(basisVectors,hourAngle,dec);

	// compute correction vector
	Vector alignCorrection(2);

	Vector* modelParameters = hourAngle >= 0 ? m_pointingModelWest : m_pointingModelEast;

	alignCorrection = (*modelParameters)[0] * basisVectors.ColumnVector(0);
	for (size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++){
		alignCorrection +=  (*modelParameters)[modelIndex] * basisVectors.ColumnVector(modelIndex);
	}

	hourAngleCor  = hourAngle - alignCorrection[0] ;
	decCor        = dec - alignCorrection[1];

}

void TpointPointingModel::ApplyInverse(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) {
	Matrix basisVectors(2,m_numOfModelParameters);

	evaluateBasis(basisVectors,hourAngle,dec);

	// compute correction vector
	Vector alignCorrection(2);

	Vector* modelParameters = hourAngle >= 0 ? m_pointingModelWest : m_pointingModelEast;

	alignCorrection = (*modelParameters)[0] * basisVectors.ColumnVector(0);
	for (size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++){
		alignCorrection +=  (*modelParameters)[modelIndex] * basisVectors.ColumnVector(modelIndex);
	}

	hourAngleCor  = hourAngle + alignCorrection[0] ;
	decCor        = dec + alignCorrection[1];
}

#define SHIFT_HA(HA)( HA > 12 ? HA - 24 : ( HA < -12 ? HA + 24 : HA) )
void TpointPointingModel::fitModel(const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide)
{

	// Count data points for each pier side
	size_t numOfPoints  = 0;
	for (auto dataPoint : syncPointArray) {
		if (!dataPoint.enabled || ( pierSide != IMCPierSide::None && dataPoint.pierSide != pierSide) )
			continue;
		numOfPoints++;
	}

	// fill a design matrix and an displacement vector
	Matrix* designMatrices        = nullptr;
	Vector* meauredDisplacements  = nullptr;


	//  design matrix
	designMatrices = new Matrix(2 * numOfPoints, m_numOfModelParameters);
	// alignmentErrorVecotr
	meauredDisplacements = new Vector (2 * numOfPoints);

	// fill design matrices
	size_t counts = 0;
	for (auto syncPoint : syncPointArray) {
		if (!syncPoint.enabled || (pierSide != IMCPierSide::None && syncPoint.pierSide != pierSide))
			continue;

		double celestialHourAngle = SHIFT_HA(syncPoint.localSiderialTime - syncPoint.celestialRA)  ;
		double telescopeHourAngle = SHIFT_HA(syncPoint.localSiderialTime - syncPoint.telecopeRA );

		// calculate design matrix
		Matrix basisVectors(2,m_numOfModelParameters);

		evaluateBasis(basisVectors,celestialHourAngle,syncPoint.celestialDEC);

		for (size_t modelIndex = 0; modelIndex < m_numOfModelParameters; modelIndex++){

			designMatrices->Element(2*counts,modelIndex)     = basisVectors[0][modelIndex];
			designMatrices->Element(2*counts + 1,modelIndex) = basisVectors[1][modelIndex];
		}

		// compute measured alignment error
		(*meauredDisplacements) [2*counts]     = celestialHourAngle - telescopeHourAngle;;
		(*meauredDisplacements) [2*counts + 1] = syncPoint.celestialDEC - syncPoint.telecopeDEC;

		counts++;
	}


	// compute pseudo inverse
	Matrix pseudoInverse((*designMatrices).Columns(),(*designMatrices).Rows());
	AnalyticalPointingModel::getPseudoInverse(pseudoInverse,(*designMatrices));

	// fit parameters
	if (pierSide == IMCPierSide::None || pierSide == IMCPierSide::West) {
		*m_pointingModelWest = pseudoInverse * *meauredDisplacements;

	}
	if (pierSide == IMCPierSide::None || pierSide == IMCPierSide::East) {
		*m_pointingModelEast = pseudoInverse * *meauredDisplacements;
	}
	delete designMatrices;
	delete meauredDisplacements;


}


void TpointPointingModel::writeObject(const String& fileName, pcl_enum pierSide)
{
	// save model parameters to disk
	IsoString fileContent;

	// west
	if (pierSide == IMCPierSide::None || pierSide == IMCPierSide::West) {
		fileContent.Append(IsoString().Format("%d,",pierSide));
		for (size_t i=0; i < this->m_numOfModelParameters; ++i){
			if (i < m_numOfModelParameters-1)
				fileContent.Append(IsoString().Format("%f,",(*m_pointingModelWest)[i]));
			else
				fileContent.Append(IsoString().Format("%f",(*m_pointingModelWest)[i]));
		}
		fileContent.Append("\n");
	}


	// east
	if (pierSide == IMCPierSide::None || pierSide == IMCPierSide::East) {
		fileContent.Append(IsoString().Format("%d,",pierSide));
		for (size_t i=0; i < this->m_numOfModelParameters; ++i){
			if (i < m_numOfModelParameters-1)
				fileContent.Append(IsoString().Format("%f,",(*m_pointingModelEast)[i]));
			else
				fileContent.Append(IsoString().Format("%f",(*m_pointingModelEast)[i]));
		}
		fileContent.Append("\n");
	}


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

void TpointPointingModel::readObject(const String& fileName)
{
	IsoStringList modelParameterList = File::ReadLines(fileName);

	for (size_t i = 0 ; i < modelParameterList.Length(); ++i) {
		IsoStringList tokens;
		modelParameterList[i].Break(tokens, ",", true);

		if ((pcl_enum) tokens[0].ToInt() == IMCPierSide::None || (pcl_enum) tokens[0].ToInt() == IMCPierSide::West){
			// west
			for (size_t j = 0; j < m_numOfModelParameters; ++j){
				(*m_pointingModelWest)[j] = tokens[j+1].ToDouble();
			}
		}
		if ((pcl_enum) tokens[0].ToInt() == IMCPierSide::None || (pcl_enum) tokens[0].ToInt() == IMCPierSide::East){
			// east
			for (size_t j = 0; j < m_numOfModelParameters; ++j){
				(*m_pointingModelEast)[j] = tokens[j+1].ToDouble();
			}
		}
	}
}


void AnalyticalPointingModel::evaluateBasis(Matrix& basisVectors, double hourAngle, double dec)
{
	if (basisVectors.Rows() != 3 || basisVectors.Columns() != 6)
	{
		throw Error( "Internal error: AnalyticalPointingModel::evaluateBasis: Matrix dimensions do not match" );
	}

	double ctc  = Cos(hourAngle * 15 * Const<double>::rad());
	double cdc  = Cos(dec * Const<double>::rad());

	double stc  = Sin(hourAngle * 15 * Const<double>::rad());
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

	evaluateBasis(basisVectors,hourAngle,dec);

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

	hourAngleCor  = ArcTan(pCorr[1],pCorr[0]) * Const<double>::deg() ;
	decCor = ArcTan(pCorr[2],Sqrt(pCorr[0]*pCorr[0] + pCorr[1] * pCorr[1])) * Const<double>::deg();

}

void AnalyticalPointingModel::ApplyInverse(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) {

}

void AnalyticalPointingModel::fitModel(const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide)
{
	//  design matrix
	Matrix designMatrix(3 * syncPointArray.Length(),m_numOfModelParameters);
	// alignmentErrorVecotr
	Vector alignmentError(3 * syncPointArray.Length());

	size_t count=0;
	for (auto syncPoint : syncPointArray) {
		double celestialHourAngle = (syncPoint.localSiderialTime - syncPoint.celestialRA)  ;
		double telescopeHourAngle = (syncPoint.localSiderialTime - syncPoint.telecopeRA)   ;

		// calculate design matrix
		Matrix basisVectors(3,m_numOfModelParameters);

		evaluateBasis(basisVectors,celestialHourAngle,syncPoint.celestialDEC);

		for (size_t modelIndex = 0; modelIndex < m_numOfModelParameters; modelIndex++){
			designMatrix[3*count]    [modelIndex] = basisVectors[0][modelIndex];
			designMatrix[3*count + 1][modelIndex] = basisVectors[1][modelIndex];
			designMatrix[3*count + 2][modelIndex] = basisVectors[2][modelIndex];
		}

		// calculate alignment error
		double ctc  = Cos(celestialHourAngle * 15 * Const<double>::rad());
		double cdc  = Cos(syncPoint.celestialDEC * Const<double>::rad());
		double stc  = Sin(celestialHourAngle * 15 * Const<double>::rad());
		double sdc  = Sin(syncPoint.celestialDEC * Const<double>::rad());

		double ctt  = Cos(telescopeHourAngle * 15 * Const<double>::rad());
		double cdt  = Cos(syncPoint.telecopeDEC * Const<double>::rad());
		double stt  = Sin(telescopeHourAngle * 15 * Const<double>::rad());
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


void AnalyticalPointingModel::writeObject(const String& fileName, pcl_enum pierSide)
{
	// save model parameters to disk
	IsoString fileContent;

	for (size_t i=0; i < this->m_numOfModelParameters; ++i){
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

		for (size_t j = 0; j < m_numOfModelParameters; ++j){
			(*m_modelParameters)[j] = tokens[j].ToDouble();
		}
	}
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Alignment.cpp - Released 2016/06/17 12:50:37 UTC
