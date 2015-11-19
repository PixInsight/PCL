//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// SkyMap.cpp - Released 2015/10/13 15:55:45 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2015 Klaus Kretzschmar
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
#include "SkyMap.h"
#include "pcl/Console.h"

namespace pcl {

// ----------------------------------------------------------------------------

CoordUtils::coord_HMS CoordUtils::convertToHMS(double coord){
	coord_HMS coordInHMS;
	coordInHMS.hour   = trunc(coord);
	coordInHMS.minute = trunc((coord - coordInHMS.hour) * 60);
	coordInHMS.second = trunc(((coord - coordInHMS.hour) * 60 - coordInHMS.minute ) * 60*100)/100.0;
	return coordInHMS;
}

CoordUtils::coord_HMS CoordUtils::parse(String coordStr){
	CoordUtils::coord_HMS result;
	StringList tokens;
	coordStr.Break(tokens,':',true);
	tokens[0].TrimLeft();
	result.hour=tokens[0].ToDouble();
	result.minute=tokens[1].ToDouble();
	result.second=tokens[2].ToDouble();
	return result;
}

double CoordUtils::convertFromHMS(const CoordUtils::coord_HMS& coord_in_HMS){
	int sign=coord_in_HMS.hour >= 0 ? 1 : -1;
	// seconds are rounded to two fractional decimals
	double coord=sign * (fabs(coord_in_HMS.hour) + coord_in_HMS.minute / 60 + coord_in_HMS.second / 3600);
	return coord;
}

double CoordUtils::convertDegFromHMS(const CoordUtils::coord_HMS& coord_in_HMS){
	return CoordUtils::convertFromHMS(coord_in_HMS) * 360.0 / 24.0 ;
}

double CoordUtils::convertRadFromHMS(const CoordUtils::coord_HMS& coord){
	return CoordUtils::convertDegFromHMS(coord) * c_rad;
}


StereoProjection::polarCoord StereoProjection::project(const StereoProjection::sphericalCoord& spherical){
	polarCoord result;

	result.r = Sin(spherical.theta) / (1.0 - Cos(spherical.theta));
	result.phi = spherical.phi;

	return result;
}

StereoProjection::polarCoord StereoProjection::projectScaled(const StereoProjection::sphericalCoord& spherical,double scale){
	polarCoord result;

	result.r = scale * Sin(spherical.theta) / (1.0 - Cos(spherical.theta));
	result.phi = spherical.phi;

	return result;
}


StereoProjection::euklidCoord StereoProjection::polarToEuklid(StereoProjection::polarCoord& polar){

	euklidCoord result;
	result.x = polar.r * Sin(-polar.phi);
	result.y = polar.r * Cos(-polar.phi);
	return result;
}



SkyMap::SkyMap(const SkyMap::filter& filter,const SkyMap::geoCoord& geoCoord):m_filter(filter), m_geoCoord(geoCoord) {


}

IsoString  SkyMap::getASDLQueryString(){

	IsoString queryStr ("SELECT  main_id AS \"Main identifier\",  RA      AS  \"RA\", DEC     AS  \"DEC\", v       AS  \"V\", sp_type as \"Spectral Type\"  FROM basic JOIN mesUBV ON mesUBV.oidref = basic.oid WHERE v < %f and mespos = 1 and DEC > %f and DEC < %f ORDER BY v ASC;");

	return IsoString().Format(queryStr.c_str(),m_filter.v_upperLimit,m_filter.dec_lowerLimit,m_filter.dec_upperLimit);
}

IsoString SkyMap::getASDLFoVQueryString(coord_d ra_center, coord_d dec_center, double width, double height, double limitStarMag){
	IsoString queryStr ("SELECT  main_id AS \"Main identifier\",  RA      AS  \"RA\", DEC     AS  \"DEC\", flux       AS  \"flux\",filter       AS  \"filter\", sp_type as \"Spectral Type\"  FROM basic JOIN flux ON flux.oidref = basic.oid WHERE flux < %f and filter = 'V' and CONTAINS(POINT('ICRS',RA,DEC),BOX('ICRS',%f,%f,%f,%f)) = 1 ORDER BY flux ASC;");
	return IsoString().Format(queryStr.c_str(),limitStarMag,ra_center,dec_center,width,height);
}

double SkyMap::getObjectAltitude(double dec, double hourAngle, double geo_lat){
	return ArcSin( Cos(Rad(geo_lat)) * Cos(Rad(dec)) * Cos(Rad(hourAngle)) + Sin(Rad(geo_lat)) * Sin(Rad(dec))  ) / Pi() * 180.0;
}

double SkyMap::getObjectAzimut(double dec, double ha, double lat){
	double rdec = Rad(dec);
	double rha  = Rad(ha);
	double rlat = Rad(lat);

	double x = Cos(rdec) * Cos(rha) * Sin(rlat) - Sin(rdec) * Cos(rlat);
	double y = Cos(rdec) * Sin(rha);
	return ArcTan2Pi(y,x) / Pi() *180.0;
}



RGBA  SkyMap::getStarColor(String spType){


	if (spType.StartsWith(String("\"O"))){
		return RGBAColor(255, 255, 255); // white
	} else if (spType.StartsWith(String("\"B"))){
		return RGBAColor(153, 153, 153); // grey
	} else if (spType.StartsWith(String("\"A"))){
		return RGBAColor(0, 0, 153); // blue
	} else if (spType.StartsWith(String("\"F"))){
		return RGBAColor(0, 153, 153); // green-blue
	} else if (spType.StartsWith(String("\"G"))){
		return RGBAColor(0, 153, 0); // green
	} else if (spType.StartsWith(String("\"K"))){
		return RGBAColor(153, 153, 0); // yellow
	} else if (spType.StartsWith(String("\"M"))){
		return RGBAColor(153, 0, 0); // red
	}
	return RGBAColor(153, 153, 0);

}

void SkyMap::plotStars(coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics& g, double limitStarMag){

	for (SkyMap::ObjectListType::iterator iter = m_objectList.Begin();iter != m_objectList.End(); ++iter) {
		// draw stars
		double hourAngle = iter->ra - lst * 360.0 / 24.0;
		double currentAlt = getObjectAltitude(iter->dec,hourAngle,geo_lat);
		double currentAz = SkyMap::getObjectAzimut(iter->dec, hourAngle, geo_lat);
		if (currentAlt > 3) {
			StereoProjection::sphericalCoord spherical;
			spherical.phi = Rad(currentAz);
			spherical.theta = Rad(90.0 + currentAlt);
			StereoProjection::polarCoord polarCoord = 	StereoProjection::projectScaled(spherical,scale);
			double xs = StereoProjection::polarToEuklid(polarCoord).x;
			double ys = StereoProjection::polarToEuklid(polarCoord).y;
			Brush brush(getStarColor(iter->spType));
			double starRadius = Sqrt(0.5 * Pow(2.5, limitStarMag - iter->v));
			pcl::Point starCenter(x + xs, y + ys);
			g.FillCircle(starCenter, starRadius, brush);
		}
	}

}

void SkyMap::plotFoVStars(coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics& g, double limitStarMag){

	for (SkyMap::ObjectListType::iterator iter = m_fovObjectList.Begin();iter != m_fovObjectList.End(); ++iter) {
		// draw stars
		double hourAngle = iter->ra;
		double currentAlt = getObjectAltitude(iter->dec,hourAngle,geo_lat);
		double currentAz = SkyMap::getObjectAzimut(iter->dec, hourAngle, geo_lat);

		StereoProjection::sphericalCoord spherical;
		spherical.phi = Rad(currentAz);
		spherical.theta = Rad(90.0 + currentAlt);
		StereoProjection::polarCoord polarCoord = 	StereoProjection::projectScaled(spherical,scale);
		double xs = StereoProjection::polarToEuklid(polarCoord).x;
		double ys = StereoProjection::polarToEuklid(polarCoord).y;
		Brush brush(getStarColor(iter->spType));
		double starRadius = Sqrt(0.5 * Pow(2.5, limitStarMag - iter->v));
		//Console().WriteLn(String().Format("x=%f, y=%f, r=%f",x+xs,y+ys,scale));
		pcl::Point starCenter(x + xs, y + ys);
		g.FillCircle(starCenter, starRadius > 15 ?  15 : starRadius , brush);

	}

}

} /* namespace pcl */
