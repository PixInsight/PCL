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
		basisVectors[0][8] = cdc * ctc + slat * tandc;
		basisVectors[1][8] = dec ;
	} else {
		basisVectors[0][8] = 0;
		basisVectors[1][8] = 0;

	}

	if (CHECK_BIT(m_modelConfig, 9)) {
		// linear term
		basisVectors[0][9] = hourAngle;
		basisVectors[1][9] = 1;
		basisVectors[0][10] = 1;
		basisVectors[1][10] = dec ;
	} else {
		basisVectors[0][9] = 0;
		basisVectors[1][9] = 0;
		basisVectors[0][10] = 0;
		basisVectors[1][10] = 0;
	}

	if (CHECK_BIT(m_modelConfig, 10)) {
		// quadratic term
		basisVectors[0][11] = hourAngle * hourAngle ;
		basisVectors[1][11] = 1;
		basisVectors[0][12] = 1;
		basisVectors[1][12] = dec * dec;
		basisVectors[0][13] = dec * hourAngle;
		basisVectors[1][13] = dec * hourAngle;
	} else {
		basisVectors[0][11] = 0;
		basisVectors[1][11] = 0;
		basisVectors[0][12] = 0;
		basisVectors[1][12] = 0;
		basisVectors[0][13] = 0;
		basisVectors[1][13] = 0;
	}

}

void TpointPointingModel::Apply(double& hourAngleCor, double& decCor, double hourAngle, double dec) {
	Matrix basisVectors(2,m_numOfModelParameters);

	evaluateBasis(basisVectors,hourAngle,dec);
	Console().WriteLn(String().Format("modelCondig=%d, ",  m_modelConfig));
	Console().WriteLn(String().Format("latitude=%f, ",  m_siteLatitude));
	//for (size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++) dumpVector(basisVectors.ColumnVector(modelIndex));
	// compute correction vector
	Vector alignCorrection(2);

	Vector* modelParameters = hourAngle >= 0 ? m_pointingModelWest : m_pointingModelEast;

	alignCorrection = (*modelParameters)[0] * basisVectors.ColumnVector(0);
	for (size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++){
		alignCorrection +=  (*modelParameters)[modelIndex] * basisVectors.ColumnVector(modelIndex);
	}

	hourAngleCor  = hourAngle - alignCorrection[0];
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

	hourAngleCor  = hourAngle + alignCorrection[0];
	decCor        = dec + alignCorrection[1];
}

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

		double celestialHourAngle = AlignmentModel::rangeShiftHourAngle(syncPoint.localSiderialTime - syncPoint.celestialRA)  ;
		double telescopeHourAngle = AlignmentModel::rangeShiftHourAngle(syncPoint.localSiderialTime - syncPoint.telecopeRA );

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
	getPseudoInverse(pseudoInverse,(*designMatrices));

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


void TpointPointingModel::writeObject(const String& fileName)
{
	// save model parameters to disk
	IsoString fileContent;

	// west
	fileContent.Append(IsoString().Format("%d,", IMCPierSide::West));
	for (size_t i=0; i < this->m_numOfModelParameters; ++i){
		if (i < m_numOfModelParameters-1)
			fileContent.Append(IsoString().Format("%f,",(*m_pointingModelWest)[i]));
		else
			fileContent.Append(IsoString().Format("%f",(*m_pointingModelWest)[i]));
	}
	fileContent.Append("\n");


	// east
	fileContent.Append(IsoString().Format("%d,",IMCPierSide::East));
	for (size_t i=0; i < this->m_numOfModelParameters; ++i){
		if (i < m_numOfModelParameters-1)
			fileContent.Append(IsoString().Format("%f,",(*m_pointingModelEast)[i]));
		else
			fileContent.Append(IsoString().Format("%f",(*m_pointingModelEast)[i]));
	}
	fileContent.Append("\n");

	// model config
	fileContent.Append(IsoString().Format("%d\n", m_modelConfig));
	// geographic site latitude
	fileContent.Append(IsoString().Format("%f\n", m_siteLatitude));

	if (File::Exists(fileName)){
		File::Remove(fileName);
	}
	File::WriteTextFile(fileName,fileContent);


}

void TpointPointingModel::readObject(const String& fileName)
{
	IsoStringList modelParameterList = File::ReadLines(fileName);

	size_t lastIndex = 0;
	for (size_t i = 0 ; i < modelParameterList.Length(); ++i) {
		lastIndex=i;
		IsoStringList tokens;
		modelParameterList[i].Break(tokens, ",", true);

		if (tokens.Length()!= m_numOfModelParameters + 1)
			break;

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

	// read model config
	m_modelConfig = modelParameterList[lastIndex].ToInt();

	// read site latitude
	m_siteLatitude = modelParameterList[++lastIndex].ToDouble();
}





// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Alignment.cpp - Released 2016/06/17 12:50:37 UTC
