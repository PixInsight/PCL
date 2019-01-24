//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0238
// ----------------------------------------------------------------------------
// Alignment.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

#include <pcl/Array.h>
#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/Matrix.h>
#include <pcl/MetaParameter.h>
#include <pcl/TimePoint.h>
#include <pcl/Vector.h>

#define CHECK_BIT( var, pos ) ((var) & (1<<(pos)))

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS XMLDocument;
class PCL_CLASS XMLElement;

struct SyncDataPoint
{
   TimePoint creationTime;
   double    localSiderialTime;
   double    celestialRA; // true position of the telescope on the sky
   double    celestialDEC;
   double    telecopeRA;  //
   double    telecopeDEC;
   pcl_enum  pierSide;
   bool      enabled;

   bool operator ==( const SyncDataPoint& rhs ) const
   {
      return   localSiderialTime == rhs.localSiderialTime
            && celestialRA       == rhs.celestialRA
            && celestialDEC      == rhs.celestialDEC
            && telecopeRA        == rhs.telecopeRA
            && pierSide          == rhs.pierSide;
   }
};

// ----------------------------------------------------------------------------

/*
 * Interface for telescope pointing models.
 */
class AlignmentModel
{
public:

   AlignmentModel() = default;

   AlignmentModel( bool modelEachPierSide, const char* modelName ) :
      m_modelEachPierSide( modelEachPierSide ),
      m_modelName( modelName )
   {
   }

   virtual ~AlignmentModel()
   {
   }

   // factory method
   static AlignmentModel* Create( const String& fileName );

   virtual void Apply( double& hourAngleCor, double& decCor, const double hourAngle, const double dec, pcl_enum pierSide)
   {
      throw Error( "Internal Error: AlignmentModel::Apply(): No implementation provided." );
   }

   virtual void ApplyInverse( double& hourAngleCor, double& decCor, const double hourAngle, const double dec, pcl_enum pierSide)
   {
      throw Error( "Internal Error: AlignmentModel::ApplyInverse(): No implementation provided." );
   }

   virtual void FitModel( const Array<SyncDataPoint>& syncPointArray )
   {
      throw Error( "Internal Error: AlignmentModel::FitModel(): No implementation provided." );
   }

   virtual void FitModel()
   {
      throw Error( "Internal Error: AlignmentModel::FitModel(): No implementation provided." );
   }

   virtual void PrintParameters()
   {
      throw Error( "Internal Error: AlignmentModel::PrintParameters(): No implementation provided." );
   }

   virtual XMLDocument* Serialize() const;
   virtual void Parse( const XMLDocument& xml );

   virtual void WriteObject( const String& fileName ) final;
   virtual void ReadObject( const String& fileName ) final;
   virtual void ReadSyncData( const String& fileName ) final;


   pcl_enum PierSideFromHourAngle( double hourAngle , bool counterWeightUpEnforced);

   Array<SyncDataPoint>& SyncDataPoints()
   {
      return m_syncData;
   }

   void AddSyncDataPoint( const SyncDataPoint& point )
   {
      m_syncData << point;
   }

   // static methods
   static Matrix PseudoInverse( const Matrix& );

   static double RangeShiftHourAngle( double hourAngle )
   {
      double shiftedHA = hourAngle;
      while ( shiftedHA < -12 )
         shiftedHA += 24;
      while ( shiftedHA >= 12 )
         shiftedHA -= 24;
      return shiftedHA;
   }

   static double RangeShiftRightAscension( double rightAscension )
   {
      double shiftedRA = rightAscension;
      while ( shiftedRA < 0 )
         shiftedRA += 24;
      while ( shiftedRA > 24 )
         shiftedRA -= 24;
      return shiftedRA;
   }

protected:

   Console              m_console;
   bool                 m_modelEachPierSide = false;
   String               m_modelName;
   Array<double>        m_residuals;
   Array<SyncDataPoint> m_syncData;
   TimePoint            m_modelCreationTime = TimePoint::Now();
   TimePoint            m_syncDataMaxCreationTime = TimePoint::Now();

   void Serialize( XMLElement* root ) const;
   AutoPointer<XMLDocument> CreateXTPMDocument() const;
   void ParseSyncData( const XMLElement& element );
   void ParseSyncDataPoint( SyncDataPoint& syncPoint, const XMLElement& element );
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
 */
class GeneralAnalyticalPointingModel : public AlignmentModel
{
private:

   constexpr static size_t modelParameters   = 11;
   constexpr static size_t maxNumOfPierSides = 2;

public:

   constexpr static const char* const modelName = "GeneralAnalytical";

   GeneralAnalyticalPointingModel()
   {
      m_pointingModelWest = new Vector( .0, m_numOfModelParameters );
      m_pointingModelEast = new Vector( .0, m_numOfModelParameters );
   }

   GeneralAnalyticalPointingModel( double siteLatitudeDeg, uint32_t modelConfig, bool modelEachPierSide ) :
      AlignmentModel( modelEachPierSide, modelName ),
      m_siteLatitude( Rad( siteLatitudeDeg ) ),
      m_modelConfig( modelConfig )
   {
      m_pointingModelWest = new Vector( .0, m_numOfModelParameters );
      m_pointingModelEast = new Vector( .0, m_numOfModelParameters );
   }

   virtual ~GeneralAnalyticalPointingModel()
   {
      delete m_pointingModelWest;
      delete m_pointingModelEast;
   }

   virtual void Apply(double& hourAngleCor, double& decCor, const double hourAngle, const double dec, pcl_enum pierSide) ;

   virtual void ApplyInverse(double& hourAngleCor, double& decCor, const double hourAngle, const double dec, pcl_enum pierSide);

   virtual void FitModel( const Array<SyncDataPoint>& syncPointArray );
   virtual void FitModel();

   // siteLatidude given in degrees
   static AlignmentModel* Create( double siteLatitude, uint32_t modelConfig, bool modelEachPierSide )
   {
      return new GeneralAnalyticalPointingModel( siteLatitude, modelConfig, modelEachPierSide );
   }

   virtual XMLDocument* Serialize() const;
   virtual void Parse( const XMLDocument& xml );

   virtual void PrintParameters();

private:

   void EvaluateBasis( Matrix& basisMatrix, double hourAngle, double dec );
   void PrintParameterVector( Vector* parameters, double residual );
   void FitModelForPierSide( const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide, double& residual );

   size_t    m_numOfModelParameters = modelParameters;
   double    m_siteLatitude = 0; // in radians
   Vector*   m_pointingModelWest = nullptr;
   Vector*   m_pointingModelEast = nullptr;
   uint32_t  m_modelConfig = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Alignment_h

// ----------------------------------------------------------------------------
// EOF Alignment.h - Released 2019-01-21T12:06:42Z
