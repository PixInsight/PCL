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
#include <pcl/AutoPointer.h>
#include <pcl/TimePoint.h>
#include <pcl/XML.h>
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

pcl_enum AlignmentModel::getPierSide(double hourAngle) {
	return (!m_modelEachPierSide || hourAngle >= 0 ) ? IMCPierSide::West : IMCPierSide::East;
}

static const double scaleArcmin = 60.0;
static const double factorHaToDeg = 15.0;


void GeneralAnalyticalPointingModel::evaluateBasis(Matrix& basisVectors, double hourAngle, double dec)
{
	// rescale hour angle to degrees
	hourAngle*=factorHaToDeg;
	if (basisVectors.Rows() != 2 || basisVectors.Columns() != static_cast<int>(m_numOfModelParameters))
	{
		throw Error( "Internal error: TpointPointingModel::evaluateBasis: Matrix dimensions do not match" );
	}
	double ctc  = Cos(hourAngle * Const<double>::rad());
	double cdc  = Cos(dec * Const<double>::rad());

	double stc  = Sin(hourAngle * Const<double>::rad());


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
		basisVectors[1][4] = ctc;
	} else {
		// polar-axis horizontal displacement
		basisVectors[0][4] = 0;
		basisVectors[1][4] = 0;
	}
	if (CHECK_BIT(m_modelConfig,5)){
		// polar-axis vertical displacement
		basisVectors[0][5] = stc * tandc;
		basisVectors[1][5] = stc;
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
		basisVectors[1][8] = 0 ;
	} else {
		basisVectors[0][8] = 0;
		basisVectors[1][8] = 0;

	}

	if (CHECK_BIT(m_modelConfig, 9)) {
		// linear term
		basisVectors[0][9] = hourAngle;
		basisVectors[1][9] = 0;
	} else {
		basisVectors[0][9] = 0;
		basisVectors[1][9] = 0;
	}

	if (CHECK_BIT(m_modelConfig, 10)) {
		// quadratic term
		basisVectors[0][10] = hourAngle * hourAngle ;
		basisVectors[1][10] = 0;
	} else {
		basisVectors[0][10] = 0;
		basisVectors[1][10] = 0;
	}

}

void GeneralAnalyticalPointingModel::Apply(double& hourAngleCor, double& decCor, double hourAngle, double dec) {
	Matrix basisVectors(2,m_numOfModelParameters);

	evaluateBasis(basisVectors,hourAngle,dec);

	// compute correction vector
	Vector alignCorrection(2);

	Vector* modelParameters = (!m_modelEachPierSide || hourAngle >= 0 ) ? m_pointingModelWest : m_pointingModelEast;

	alignCorrection = (*modelParameters)[0] * basisVectors.ColumnVector(0);
	for (size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++){
		alignCorrection +=  (*modelParameters)[modelIndex] * basisVectors.ColumnVector(modelIndex);
	}

	hourAngleCor  = hourAngle - alignCorrection[0] / factorHaToDeg;
	decCor        = dec - alignCorrection[1];
}

void GeneralAnalyticalPointingModel::ApplyInverse(double& hourAngleCor, double& decCor, const double hourAngle, const double dec) {
	Matrix basisVectors(2,m_numOfModelParameters);

	evaluateBasis(basisVectors,hourAngle,dec);

	// compute correction vector
	Vector alignCorrection(2);

	Vector* modelParameters = (!m_modelEachPierSide || hourAngle >= 0) ? m_pointingModelWest : m_pointingModelEast;

	alignCorrection = (*modelParameters)[0] * basisVectors.ColumnVector(0);
	for (size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++){
		alignCorrection +=  (*modelParameters)[modelIndex] * basisVectors.ColumnVector(modelIndex);
	}

	hourAngleCor  = hourAngle + alignCorrection[0] / factorHaToDeg;
	decCor        = dec + alignCorrection[1];
}

void GeneralAnalyticalPointingModel::fitModel() {
	fitModel(m_syncData);
}

void GeneralAnalyticalPointingModel::fitModel(const Array<SyncDataPoint>& syncPointArray) {
	m_modelCreationTime = TimePoint::Now();
	double residual=0;
	if (!m_modelEachPierSide){
		fitModelForPierSide(syncPointArray, IMCPierSide::None, residual);
		m_residuals.Add(residual);
	} else {
		fitModelForPierSide(syncPointArray, IMCPierSide::West, residual);
		m_residuals.Add(residual);
		fitModelForPierSide(syncPointArray, IMCPierSide::East, residual);
		m_residuals.Add(residual);
	}
}

void GeneralAnalyticalPointingModel::fitModelForPierSide(const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide, double& residual)
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
	Vector* measuredDisplacements  = nullptr;

	//  design matrix
	designMatrices = new Matrix(2 * numOfPoints, m_numOfModelParameters);
	// alignmentErrorVecotr
	measuredDisplacements = new Vector (2 * numOfPoints);

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
		(*measuredDisplacements) [2*counts]     = (celestialHourAngle - telescopeHourAngle)*factorHaToDeg;
		(*measuredDisplacements) [2*counts + 1] = syncPoint.celestialDEC - syncPoint.telecopeDEC;

		counts++;
	}


	// compute pseudo inverse
	Matrix pseudoInverse((*designMatrices).Columns(),(*designMatrices).Rows());
	getPseudoInverse(pseudoInverse,(*designMatrices));

	// fit parameters
	if (pierSide == IMCPierSide::None || pierSide == IMCPierSide::West) {
		*m_pointingModelWest = pseudoInverse * *measuredDisplacements;
		// compute residual
		Vector residualVector = (*designMatrices) * *m_pointingModelWest - *measuredDisplacements;
		residual = residualVector.Norm() / m_pointingModelWest->Norm();

	}
	if (pierSide == IMCPierSide::East) {
		*m_pointingModelEast = pseudoInverse * *measuredDisplacements;
		// compute residual
		Vector residualVector = (*designMatrices) * *m_pointingModelEast - *measuredDisplacements;
		residual = residualVector.Norm() / m_pointingModelEast->Norm();
	}

	delete designMatrices;
	delete measuredDisplacements;
}


XMLDocument* GeneralAnalyticalPointingModel::Serialize() const {

	AutoPointer<XMLDocument> xml = new XMLDocument;
	xml->SetXML( "1.0", "UTF-8" );
	*xml << new XMLComment( "\nPixInsight XML Telescope Pointing Model Format - XTPM version 1.0"
			                "\nCreated with PixInsight software - http://pixinsight.com/"
			                "\n" );

	XMLElement* root = new XMLElement( "xtpm", XMLAttributeList()
			<< XMLAttribute( "version", "1.0"));

	xml->SetRootElement( root );

	*(new XMLElement( *root, "CreationTime" )) << new XMLText( m_modelCreationTime.ToString() );

	*(new XMLElement( *root, "GeographicLatitude" )) << new XMLText( String(m_siteLatitude) );

	*(new XMLElement( *root, "Configuration" )) << new XMLText( String(m_modelConfig) );

	*(new XMLElement( *root, "ModelParameters", XMLAttributeList() << XMLAttribute("PierSide", m_modelEachPierSide ? "West" : "None" ) ))  << new XMLText( String().ToCommaSeparated( *m_pointingModelWest ) );

	if (m_modelEachPierSide) {
		*(new XMLElement( *root, "ModelParameters", XMLAttributeList() << XMLAttribute("PierSide", "East" )  ))  << new XMLText( String().ToCommaSeparated( *m_pointingModelEast ) );
	}

	if (m_syncData.Length()!=0) {
		XMLElement* list = new XMLElement(*root, String("SyncDataList"));
		for (auto syncDataPoint : m_syncData) {
			XMLElement* listElement =  new XMLElement(*list, String("SyncDataPoint"), XMLAttributeList() << XMLAttribute( "CreationTime", String(syncDataPoint.creationTime.ToString())));
			*(new XMLElement( *listElement, "LocalSiderialTime" )) << new XMLText( String(syncDataPoint.localSiderialTime) );
			*(new XMLElement( *listElement, "CelestialRA" )) << new XMLText( String(syncDataPoint.celestialRA) );
			*(new XMLElement( *listElement, "CelestialDEC" )) << new XMLText( String(syncDataPoint.celestialDEC) );
			*(new XMLElement( *listElement, "TelescopeRA" )) << new XMLText( String(syncDataPoint.telecopeRA) );
			*(new XMLElement( *listElement, "TelescopeDEC" )) << new XMLText( String(syncDataPoint.telecopeDEC) );
			*(new XMLElement( *listElement, "Pierside" )) << new XMLText( String(syncDataPoint.pierSide) );
			*(new XMLElement( *listElement, "Enabled" )) << new XMLText( String(syncDataPoint.enabled) );
		}
	}

	return xml.Release();
}

void GeneralAnalyticalPointingModel::ParseSyncDataPoint(SyncDataPoint& syncPoint, const XMLElement& element) {
	for (const XMLNode& node : element) {
		const XMLElement& element = static_cast<const XMLElement&>(node);
		try {
			if (element.Name() == "LocalSiderialTime") {
				syncPoint.localSiderialTime = element.Text().Trimmed().ToDouble();
			} else if (element.Name() == "CelestialRA") {
				syncPoint.celestialRA = element.Text().Trimmed().ToDouble();
			} else if (element.Name() == "CelestialDEC") {
				syncPoint.celestialDEC = element.Text().Trimmed().ToDouble();
			} else if (element.Name() == "TelescopeRA") {
				syncPoint.telecopeRA = element.Text().Trimmed().ToDouble();
			} else if (element.Name() == "TelescopeDEC") {
				syncPoint.telecopeDEC = element.Text().Trimmed().ToDouble();
			} else if (element.Name() == "Pierside") {
				syncPoint.pierSide = element.Text().Trimmed().ToInt();
			} else if (element.Name() == "Enabled") {
				syncPoint.enabled = element.Text().Trimmed().ToBool();
			}
		} catch (...) {
			throw;
		}
	}

}

void  GeneralAnalyticalPointingModel::ParseSyncData(const XMLElement& syncDataList) {
	for (const XMLNode& node : syncDataList) {
		const XMLElement& element = static_cast<const XMLElement&>(node);
		SyncDataPoint syncDataPoint;
		try {
			if (element.Name() == "SyncDataPoint") {
				syncDataPoint.creationTime = TimePoint(element.AttributeValue("CreationTime"));
				ParseSyncDataPoint(syncDataPoint, element);
			}
			m_syncData.Append(syncDataPoint);
		} catch (...) {
			throw;
		}
	}
}


void GeneralAnalyticalPointingModel::Parse( const XMLDocument& xml) {

	if ( xml.RootElement() == nullptr )
		throw Error( "The XML document has no root element." );
	if ( xml.RootElement()->Name() != "xtpm" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
		throw Error( "Not an XTPM version 1.0 document." );


	const XMLElement& root = *xml.RootElement();
	for (const XMLNode& node : root) {
		const XMLElement& element = static_cast<const XMLElement&>(node);

		try {
			if (element.Name() == "GeographicLatitude") {
				m_siteLatitude = element.Text().Trimmed().ToDouble();
			} else if (element.Name() == "Configuration") {
				m_modelConfig = element.Text().Trimmed().ToInt();
			} else if (element.Name() == "ModelParameters") {
				m_modelEachPierSide = element.AttributeValue("PierSide") != "None";
				if (element.AttributeValue("PierSide") == "West" || element.AttributeValue("PierSide") == "None") {
					*m_pointingModelWest = ParseListOfRealValues(element);
				} else {
					*m_pointingModelEast = ParseListOfRealValues(element);
				}
			} else if (element.Name() == "SyncDataList") {
				ParseSyncData(element);
			}
		} catch (...) {
			throw;
		}
	}
}

void GeneralAnalyticalPointingModel::writeObject(const String& fileName)
{
	// save model parameters to disk
	if (File::Exists(fileName)){
		File::Remove(fileName);
	}

	// write xtpm file
	XMLDocument* xml = Serialize();
	xml->EnableAutoFormatting();
	String xmlFileName = fileName;
	xml->SerializeToFile( xmlFileName );
}

void GeneralAnalyticalPointingModel::readObject(const String& fileName)
{
	IsoString text = File::ReadTextFile( fileName );
	XMLDocument xml;
	xml.SetParserOption( XMLParserOption::IgnoreComments );
	xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
	xml.Parse( text.UTF8ToUTF16() );
	Parse( xml );
}


void GeneralAnalyticalPointingModel::printParameterVector(Vector* parameters, double residual){
	m_console.WriteLn( String().Format("<end>* fitting residual :.. ................................................%+.2f ",residual));

	m_console.WriteLn( String().Format("<end>** hour angle offset:..................................................%+.2f (arcmin)",(*parameters)[0]*scaleArcmin));
	m_console.WriteLn( String().Format("<end>** declination offset:.................................................%+.2f (arcmin)",(*parameters)[1]*scaleArcmin));
	double poleSep = Sqrt((*parameters)[4] * (*parameters)[4] + (*parameters)[5]* (*parameters)[5]);
	double poleAngle = ArcCos(- (*parameters)[4] / poleSep);
	m_console.WriteLn( String().Format("<end>** angular separation between true and instrumental pole:..............%+.2f (arcmin)",poleSep*scaleArcmin));
	m_console.WriteLn( String().Format("<end>** angle between meridian and line of true and instrumental pole:......%+.2f (deg)",Deg(poleAngle)));
	m_console.WriteLn( String().Format("<end>** mis-alignment of optical and mechanical axes:.......................%+.2f (arcmin)",(*parameters)[2]*scaleArcmin));
	m_console.WriteLn( String().Format("<end>** polar/declination axis non-orthogonality:...........................%+.2f (arcmin)",-(*parameters)[3]*scaleArcmin));
	m_console.WriteLn( String().Format("<end>** tube flexure - droop away from zenith:..............................%+.2f (arcmin)",(*parameters)[6]*scaleArcmin));
	m_console.WriteLn( String().Format("<end>** bending of declination axis:........................................%+.2f (arcmin)",(*parameters)[8]*scaleArcmin));
	m_console.WriteLn( String().Format("<end>** linear hour angle scale error:......................................%+.2f (arcmin)",(*parameters)[9]*scaleArcmin));
	m_console.WriteLn( String().Format("<end>** quadratic hour angle scale error....................................%+.2f (arcmin)",(*parameters)[10]*scaleArcmin));

}

void GeneralAnalyticalPointingModel::printParameters() {

	m_console.NoteLn( "<end><cbr><br> Analytical Pointing Model Parameters" );
	m_console.WriteLn("The parameters refer to the general analytical telescope pointing model as described by Marc W. Buie (2013)");
	m_console.WriteLn("in his paper http://www.boulder.swri.edu/~buie/idl/downloads/pointing/pointing.pdf.");


	if (m_modelEachPierSide) m_console.WriteLn("<end><cbr><br>* Pierside: West");
	printParameterVector(m_pointingModelWest, m_residuals[0]);
	if (m_modelEachPierSide){
		m_console.WriteLn("<end><cbr><br>* Pierside: East");
		printParameterVector(m_pointingModelEast, m_residuals[1]);
	}
}



// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Alignment.cpp - Released 2016/06/17 12:50:37 UTC
