//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0232
// ----------------------------------------------------------------------------
// Alignment.cpp - Released 2018-12-12T09:25:25Z
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

#include "Alignment.h"
#include "INDIMountParameters.h"

#include <pcl/Algebra.h>
#include <pcl/Console.h>
#include <pcl/Constants.h>
#include <pcl/Math.h>
#include <pcl/TimePoint.h>
#include <pcl/XML.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static bool TryToDouble( double& value, IsoString::const_iterator p )
{
   IsoString::iterator endptr = nullptr;
   errno = 0;
   double val = ::strtod( p, &endptr );
   if ( errno == 0 && (endptr == nullptr || *endptr == '\0') )
   {
      value = val;
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

static Vector ParseListOfRealValues( IsoString& text, size_type start, size_type end, size_type minCount = 0, size_type maxCount = ~size_type( 0 ) )
{
   Array<double> v;
   for ( size_type i = start, j; i < end; ++i )
   {
      for ( j = i; j < end; ++j )
         if ( text[j] == ',' )
            break;
      text[j] = '\0';
      double x;
      if ( !TryToDouble( x, text.At( i ) ) )
         throw Error( "Parsing real numeric list: Invalid floating point numeric literal \'" + IsoString( text.At( i ) ) + "\'" );
      if ( v.Length() == maxCount )
         throw Error( "Parsing real numeric list: Too many items." );
      v << x;
      i = j;
   }
   if ( v.Length() < minCount )
      throw Error( "Parsing real numeric list: Too few items." );
   return Vector( v.Begin(), int( v.Length() ) );
}

static Vector ParseListOfRealValues( const XMLElement& element, size_type minCount = 0, size_type maxCount = ~size_type( 0 ) )
{
   IsoString text = IsoString( element.Text().Trimmed() );
   return ParseListOfRealValues( text, 0, text.Length(), minCount, maxCount );
}

// ----------------------------------------------------------------------------

static void WarnOnUnknownChildElement( const XMLElement& element, const String& parsingWhatElement )
{
   XMLParseError e( element,
         "Parsing " + parsingWhatElement + " element",
         "Skipping unknown \'" + element.Name() + "\' child element." );
   Console().WarningLn( "<end><cbr>** Warning: " + e.Message() );
}

// ----------------------------------------------------------------------------

// static void DumpMatrix( const Matrix& matrix )
// {
//    Console console;
//    console.WriteLn( "Matrix:" );
//    for ( int i = 0; i < matrix.Rows(); ++i )
//    {
//       for ( int j = 0; j < matrix.Columns(); ++j )
//          console.Write( String().Format( "m[%d,%d]=%6.6f, ", i, j, matrix[i][j] ) );
//       console.WriteLn();
//    }
// }

// ----------------------------------------------------------------------------

// static void DumpVector( const Vector& vector )
// {
//    Console console;
//    console.WriteLn( "Vector:" );
//    for ( int j = 0; j < vector.Length(); ++j )
//       console.Write( String().Format( "v[%d]=%6.6f, ", j, vector[j] ) );
//    console.WriteLn();
// }

// ----------------------------------------------------------------------------

Matrix AlignmentModel::PseudoInverse( const Matrix& matrix )
{
   SVD svd( matrix );
   Matrix WInverse( matrix.Columns(), matrix.Columns() );
   for ( int j = 0; j < matrix.Columns(); ++j )
      for ( int i = 0; i < matrix.Columns(); ++i )
         WInverse[i][j] = (i == j && Abs( svd.W[i] ) > 1e-15) ? 1/svd.W[i] : 0.0;
   return svd.V * WInverse * svd.U.Transpose();
}

// ----------------------------------------------------------------------------

pcl_enum AlignmentModel::PierSideFromHourAngle( double hourAngle, bool counterWeightUpEnforced)
{
   pcl_enum pierSideWest = counterWeightUpEnforced ? IMCPierSide::East : IMCPierSide::West;
   pcl_enum pierSideEast = counterWeightUpEnforced ? IMCPierSide::West : IMCPierSide::East;
   return (!m_modelEachPierSide || hourAngle <= 0) ? pierSideWest : pierSideEast;
}

// ----------------------------------------------------------------------------

AutoPointer<XMLDocument> AlignmentModel::CreateXTPMDocument() const
{
   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetXML( "1.0", "UTF-8" );
   *xml << new XMLComment( "\nPixInsight XML Telescope Pointing Model Format - XTPM version 1.0"
                           "\nCreated with PixInsight software - http://pixinsight.com/"
                           "\n" );
   xml->SetRootElement( new XMLElement( "xtpm", XMLAttributeList() << XMLAttribute( "version", "1.0" ) ) );
   return xml;
}

// ----------------------------------------------------------------------------

void AlignmentModel::Serialize(XMLElement* root) const
{
   if ( root == nullptr )
      throw Error( "Internal Error: AlignmentModel::Serialize(): Invalid root pointer" );

   if ( !m_syncData.IsEmpty() )
   {
      XMLElement* list = new XMLElement( *root, String( "SyncDataList" ) );
      for ( auto syncDataPoint : m_syncData )
      {
         XMLElement* listElement = new XMLElement( *list, "SyncDataPoint",
                     XMLAttributeList() << XMLAttribute( "CreationTime", syncDataPoint.creationTime.ToString() ) );
         *(new XMLElement( *listElement, "LocalSiderialTime" )) << new XMLText( String( syncDataPoint.localSiderialTime ) );
         *(new XMLElement( *listElement, "CelestialRA" )) << new XMLText( String( syncDataPoint.celestialRA ) );
         *(new XMLElement( *listElement, "CelestialDEC" )) << new XMLText( String( syncDataPoint.celestialDEC ) );
         *(new XMLElement( *listElement, "TelescopeRA" )) << new XMLText( String( syncDataPoint.telecopeRA ) );
         *(new XMLElement( *listElement, "TelescopeDEC" )) << new XMLText( String( syncDataPoint.telecopeDEC ) );
         *(new XMLElement( *listElement, "Pierside" )) << new XMLText( String( syncDataPoint.pierSide ) );
         *(new XMLElement( *listElement, "Enabled" )) << new XMLText( String( syncDataPoint.enabled ) );
      }
   }
}

// ----------------------------------------------------------------------------

XMLDocument* AlignmentModel::Serialize() const
{
   AutoPointer<XMLDocument> xml = CreateXTPMDocument();
   XMLElement* mutableRoot = const_cast<XMLElement*>( xml->RootElement() );
   Serialize( mutableRoot );
   return xml.Release();
}

// ----------------------------------------------------------------------------

void AlignmentModel::ParseSyncDataPoint( SyncDataPoint& syncPoint, const XMLElement& element )
{
   for ( const XMLNode& node : element )
   {
      const XMLElement& element = static_cast<const XMLElement&>( node );
      try
      {
         if ( element.Name() == "LocalSiderialTime" )
            syncPoint.localSiderialTime = element.Text().Trimmed().ToDouble();
         else if ( element.Name() == "CelestialRA" )
            syncPoint.celestialRA = element.Text().Trimmed().ToDouble();
         else if ( element.Name() == "CelestialDEC" )
            syncPoint.celestialDEC = element.Text().Trimmed().ToDouble();
         else if ( element.Name() == "TelescopeRA" )
            syncPoint.telecopeRA = element.Text().Trimmed().ToDouble();
         else if ( element.Name() == "TelescopeDEC" )
            syncPoint.telecopeDEC = element.Text().Trimmed().ToDouble();
         else if ( element.Name() == "Pierside" )
            syncPoint.pierSide = element.Text().Trimmed().ToInt();
         else if ( element.Name() == "Enabled" )
            syncPoint.enabled = element.Text().Trimmed().ToBool();
         else
            WarnOnUnknownChildElement( element, "xtpm root" );
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

void AlignmentModel::ParseSyncData( const XMLElement& syncDataList )
{
   for ( const XMLNode& node : syncDataList )
   {
      const XMLElement& element = static_cast<const XMLElement&>( node );
      SyncDataPoint syncDataPoint;
      try
      {
         if ( element.Name() == "SyncDataPoint" )
         {
            syncDataPoint.creationTime = TimePoint( element.AttributeValue( "CreationTime" ) );
            if ( syncDataPoint.creationTime > m_syncDataMaxCreationTime )
               m_syncDataMaxCreationTime = syncDataPoint.creationTime;
            ParseSyncDataPoint( syncDataPoint, element );
            m_syncData.Append( syncDataPoint );
         }
         else
            WarnOnUnknownChildElement( element, "xtpm root" );
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }
   if ( m_syncData.IsEmpty() )
      throw Error("Missing required sync data point.");
   if ( m_syncDataMaxCreationTime > m_modelCreationTime )
      Console().WarningLn( "<end><cbr>** Warning: Telescope pointing model is outdated. There are new sync data points, consider re-fitting the model." );
}

// ----------------------------------------------------------------------------

void AlignmentModel::Parse( const XMLDocument& xml )
{
   if ( xml.RootElement() == nullptr ) // ?! cannot happen
      throw Error( "The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xtpm" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XTPM version 1.0 document." );

   const XMLElement& root = *xml.RootElement();
   for ( const XMLNode& node : root )
   {
      const XMLElement& element = static_cast<const XMLElement&>( node );
      try
      {
         if ( element.Name() == "SyncDataList" )
            ParseSyncData( element );
         else if ( element.Name() == "ModelName" )
            m_modelName = element.Text().Trimmed();
         else
            WarnOnUnknownChildElement( element, "xtpm root" );
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }
   if ( m_syncData.IsEmpty() )
      throw Error( "Missing required sync data list." );
}

// ----------------------------------------------------------------------------

void AlignmentModel::WriteObject( const String& fileName )
{
   AutoPointer<XMLDocument> xml = Serialize();
   xml->EnableAutoFormatting();
   xml->SerializeToFile( fileName );
}

// ----------------------------------------------------------------------------

void AlignmentModel::ReadObject( const String& fileName )
{
   IsoString text = File::ReadTextFile( fileName );
   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( text.UTF8ToUTF16() );
   Parse( xml );
}

// ----------------------------------------------------------------------------

void AlignmentModel::ReadSyncData( const String& fileName )
{
   IsoString text = File::ReadTextFile( fileName );
   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( text.UTF8ToUTF16() );
   if ( xml.RootElement()->Name() != "xtpm" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XTPM version 1.0 document." );

   const XMLElement& root = *xml.RootElement();
   for ( const XMLNode& node : root )
   {
      const XMLElement& element = static_cast<const XMLElement&>( node );
      try
      {
         if ( element.Name() == "SyncDataList" )
            ParseSyncData( element );
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }
   if ( m_syncData.IsEmpty() )
      throw Error( "Missing required sync data list." );
}

// ----------------------------------------------------------------------------

AlignmentModel* AlignmentModel::Create( const String& fileName )
{
   if ( fileName.IsEmpty() )
      throw Error( "Pointing model: file name is empty." );
   if ( !File::Exists( fileName ) )
      throw Error( "PointingModel: xtpm file does not exist." );

   IsoString text = File::ReadTextFile( fileName );
   XMLDocument xml;
   xml.SetParserOption( XMLParserOption::IgnoreComments );
   xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
   xml.Parse( text.UTF8ToUTF16() );

   if ( xml.RootElement()->Name() != "xtpm" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XTPM version 1.0 document." );

   AlignmentModel* result;
   String modelName;
   const XMLElement& root = *xml.RootElement();
   for ( const XMLNode& node : root )
   {
      const XMLElement& element = static_cast<const XMLElement&>( node );
      try
      {
         if ( element.Name() == "ModelName" )
            modelName = element.Text().Trimmed();
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }
   if ( modelName == GeneralAnalyticalPointingModel::modelName )
      result = new GeneralAnalyticalPointingModel();
   else
      result = new AlignmentModel();
   result->Parse( xml );
   return result;
}

// ----------------------------------------------------------------------------

static const double scaleArcmin = 60.0;
static const double factorHaToDeg = 15.0;

void GeneralAnalyticalPointingModel::EvaluateBasis( Matrix& basisVectors, double hourAngle, double dec )
{
   // rescale hour angle to degrees
   hourAngle *= factorHaToDeg;
   if ( basisVectors.Rows() != 2 || basisVectors.Columns() != static_cast<int>( m_numOfModelParameters ) )
      throw Error( "Internal error: PointingModel::EvaluateBasis: Matrix dimensions do not match" );

   double ctc   = Cos( Rad( hourAngle ) );
   double cdc   = Cos( Rad( dec ) );
   double stc   = Sin( Rad( hourAngle ) );
   double sdc   = Sin( Rad( dec ) );

   double secdc = 1/Cos( Rad( dec ) );
   double tandc = Tan( Rad( dec ) );

   double clat  = Cos( m_siteLatitude );
   double slat  = Sin( m_siteLatitude );

   if ( CHECK_BIT( m_modelConfig, 1 ) )
   {
      // zero-point offset in ra readout
      basisVectors[0][0] = 1;
      basisVectors[1][0] = 0;
      // zero-point offset in dec readout
      basisVectors[0][1] = 0;
      basisVectors[1][1] = 1;
   }
   else
   {
      basisVectors[0][0] = 0;
      basisVectors[1][0] = 0;
      basisVectors[0][1] = 0;
      basisVectors[1][1] = 0;
   }

   if ( CHECK_BIT( m_modelConfig, 2 ) )
   {
      // collimation error
      basisVectors[0][2] = secdc;
      basisVectors[1][2] = 0;
   }
   else
   {
      // collimation error
      basisVectors[0][2] =  0;
      basisVectors[1][2] =  0;
   }

   if ( CHECK_BIT( m_modelConfig, 3 ) )
   {
      // non-perpendicular dec-ra axis error
      basisVectors[0][3] = tandc;
      basisVectors[1][3] = 0;
   }
   else
   {
      // non-perpendicular dec-ra axis error
      basisVectors[0][3] =  0;
      basisVectors[1][3] =  0;
   }

   if ( CHECK_BIT(m_modelConfig, 4 ) )
   {
      // polar-axis horizontal displacement
      basisVectors[0][4] = -ctc * tandc;
      basisVectors[1][4] = ctc;
   }
   else
   {
      // polar-axis horizontal displacement
      basisVectors[0][4] = 0;
      basisVectors[1][4] = 0;
   }

   if ( CHECK_BIT( m_modelConfig, 5 ) )
   {
      // polar-axis vertical displacement
      basisVectors[0][5] = stc * tandc;
      basisVectors[1][5] = stc;
   }
   else
   {
      // polar-axis vertical displacement
      basisVectors[0][5] = 0;
      basisVectors[1][5] = 0;
   }

   if ( CHECK_BIT( m_modelConfig, 6 ) )
   {
      // tube flexure
      basisVectors[0][6] = clat * stc * secdc;
      basisVectors[1][6] = clat * ctc * sdc - slat * cdc;
   }
   else
   {
      basisVectors[0][6] = 0;
      basisVectors[1][6] = 0;
   }

   if ( CHECK_BIT( m_modelConfig, 7 ) )
   {
      // fork flexure
      basisVectors[0][7] = 0;
      basisVectors[1][7] = ctc;
   }
   else
   {
      basisVectors[0][7] = 0;
      basisVectors[1][7] = 0;
   }

   if ( CHECK_BIT( m_modelConfig, 8 ) )
   {
      // delta-axis flexure
      basisVectors[0][8] = cdc * ctc + slat * tandc;
      basisVectors[1][8] = 0 ;
   }
   else
   {
      basisVectors[0][8] = 0;
      basisVectors[1][8] = 0;
   }

   if ( CHECK_BIT( m_modelConfig, 9 ) )
   {
      // linear term
      basisVectors[0][9] = hourAngle;
      basisVectors[1][9] = 0;
   }
   else
   {
      basisVectors[0][9] = 0;
      basisVectors[1][9] = 0;
   }

   if ( CHECK_BIT( m_modelConfig, 10 ) )
   {
      // quadratic term
      basisVectors[0][10] = hourAngle * hourAngle ;
      basisVectors[1][10] = 0;
   }
   else
   {
      basisVectors[0][10] = 0;
      basisVectors[1][10] = 0;
   }
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::Apply( double& hourAngleCor, double& decCor, double hourAngle, double dec, pcl_enum pierSide )
{
   Matrix basisVectors( 2, m_numOfModelParameters );

   EvaluateBasis( basisVectors, hourAngle, dec );

   // compute correction vector
   Vector alignCorrection( 2 );
   Vector* modelParameters = (!m_modelEachPierSide || pierSide == IMCPierSide::West) ? m_pointingModelWest : m_pointingModelEast;

   alignCorrection = (*modelParameters)[0] * basisVectors.ColumnVector( 0 );
   for ( size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++ )
      alignCorrection += (*modelParameters)[modelIndex] * basisVectors.ColumnVector( modelIndex );

   hourAngleCor = hourAngle - alignCorrection[0]/factorHaToDeg;
   decCor       = dec - alignCorrection[1];
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::ApplyInverse( double& hourAngleCor, double& decCor, const double hourAngle, const double dec, pcl_enum pierSide )
{
   Matrix basisVectors( 2, m_numOfModelParameters );

   EvaluateBasis( basisVectors, hourAngle, dec );

   // compute correction vector
   Vector alignCorrection( 2 );
   Vector* modelParameters = (!m_modelEachPierSide ||  pierSide == IMCPierSide::West) ? m_pointingModelWest : m_pointingModelEast;

   alignCorrection = (*modelParameters)[0] * basisVectors.ColumnVector( 0 );
   for ( size_t modelIndex = 1; modelIndex < m_numOfModelParameters; modelIndex++ )
      alignCorrection +=  (*modelParameters)[modelIndex] * basisVectors.ColumnVector( modelIndex );

   hourAngleCor = hourAngle + alignCorrection[0]/factorHaToDeg;
   decCor       = dec + alignCorrection[1];
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::FitModel()
{
   FitModel( m_syncData );
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::FitModel( const Array<SyncDataPoint>& syncPointArray )
{
   m_modelCreationTime = TimePoint::Now();
   double residual = 0;
   if ( !m_modelEachPierSide )
   {
      FitModelForPierSide( syncPointArray, IMCPierSide::None, residual );
      m_residuals.Add( residual );
   }
   else
   {
      FitModelForPierSide( syncPointArray, IMCPierSide::West, residual );
      m_residuals.Add( residual );
      FitModelForPierSide( syncPointArray, IMCPierSide::East, residual );
      m_residuals.Add( residual );
   }
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::FitModelForPierSide( const Array<SyncDataPoint>& syncPointArray, pcl_enum pierSide, double& residual )
{
   // Count data points for each pier side
   size_t numOfPoints  = 0;
   for ( auto dataPoint : syncPointArray )
   {
      if ( !dataPoint.enabled || pierSide != IMCPierSide::None && dataPoint.pierSide != pierSide )
         continue;
      numOfPoints++;
   }

   if ( numOfPoints == 0 )
      throw Error( "GeneralAnalyticalPointingModel::FitModelForPierSide: Missing required sync data." );

   // fill a design matrix and an displacement vector
   Matrix* designMatrices = nullptr;
   Vector* measuredDisplacements = nullptr;

   // design matrix
   designMatrices = new Matrix( 2*numOfPoints, m_numOfModelParameters );
   // alignmentErrorVecotr
   measuredDisplacements = new Vector ( 2*numOfPoints );

   // fill design matrices
   size_t counts = 0;
   for ( auto syncPoint : syncPointArray )
   {
      if ( !syncPoint.enabled || pierSide != IMCPierSide::None && syncPoint.pierSide != pierSide )
         continue;

      double celestialHourAngle = AlignmentModel::RangeShiftHourAngle( syncPoint.localSiderialTime - syncPoint.celestialRA );
      double telescopeHourAngle = AlignmentModel::RangeShiftHourAngle( syncPoint.localSiderialTime - syncPoint.telecopeRA );

      // calculate design matrix
      Matrix basisVectors( 2, m_numOfModelParameters );

      EvaluateBasis( basisVectors, celestialHourAngle, syncPoint.celestialDEC );

      for ( size_t modelIndex = 0; modelIndex < m_numOfModelParameters; modelIndex++ )
      {
         designMatrices->Element( 2*counts, modelIndex )     = basisVectors[0][modelIndex];
         designMatrices->Element( 2*counts + 1, modelIndex ) = basisVectors[1][modelIndex];
      }

      // compute measured alignment error
      (*measuredDisplacements)[2*counts]     = (celestialHourAngle - telescopeHourAngle)*factorHaToDeg;
      (*measuredDisplacements)[2*counts + 1] = syncPoint.celestialDEC - syncPoint.telecopeDEC;

      counts++;
   }

   // compute pseudo inverse
   Matrix pseudoInverse = PseudoInverse( *designMatrices );

   // fit parameters
   if ( pierSide == IMCPierSide::None || pierSide == IMCPierSide::West )
   {
      *m_pointingModelWest = pseudoInverse * *measuredDisplacements;
      // compute residual
      Vector residualVector = (*designMatrices) * *m_pointingModelWest - *measuredDisplacements;
      residual = residualVector.Norm()/m_pointingModelWest->Norm();
   }
   if ( pierSide == IMCPierSide::East )
   {
      *m_pointingModelEast = pseudoInverse * *measuredDisplacements;
      // compute residual
      Vector residualVector = (*designMatrices) * *m_pointingModelEast - *measuredDisplacements;
      residual = residualVector.Norm()/m_pointingModelEast->Norm();
   }

   delete designMatrices;
   delete measuredDisplacements;
}

// ----------------------------------------------------------------------------

XMLDocument* GeneralAnalyticalPointingModel::Serialize() const
{
   AutoPointer<XMLDocument> xml = CreateXTPMDocument();

   XMLElement* root = const_cast<XMLElement*>( xml->RootElement() );

   *(new XMLElement( *root, "ModelName" )) << new XMLText( m_modelName );
   *(new XMLElement( *root, "CreationTime" )) << new XMLText( m_modelCreationTime.ToString() );
   *(new XMLElement( *root, "GeographicLatitude" )) << new XMLText( String( m_siteLatitude ) );
   *(new XMLElement( *root, "Configuration" )) << new XMLText( String( m_modelConfig ) );
   *(new XMLElement( *root, "ModelParameters", XMLAttributeList() << XMLAttribute("PierSide", m_modelEachPierSide ? "West" : "None" ) ))
         << new XMLText( String().ToCommaSeparated( *m_pointingModelWest ) );

   if ( m_modelEachPierSide )
      *(new XMLElement( *root, "ModelParameters", XMLAttributeList() << XMLAttribute( "PierSide", "East" ) ))
         << new XMLText( String().ToCommaSeparated( *m_pointingModelEast ) );

   AlignmentModel::Serialize( root );

   return xml.Release();
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::Parse( const XMLDocument& xml )
{
   if ( xml.RootElement() == nullptr ) // ?! cannot happen
      throw Error( "Internal error: The XML document has no root element." );
   if ( xml.RootElement()->Name() != "xtpm" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
      throw Error( "Not an XTPM version 1.0 document." );

   const XMLElement& root = *xml.RootElement();
   for ( const XMLNode& node : root )
   {
      const XMLElement& element = static_cast<const XMLElement&>(node);
      try
      {
         if ( element.Name() == "ModelName" )
            m_modelName = element.Text().Trimmed();
         else if ( element.Name() == "GeographicLatitude" )
            m_siteLatitude = element.Text().Trimmed().ToDouble();
         else if ( element.Name() == "Configuration" )
            m_modelConfig = element.Text().Trimmed().ToUInt();
         else if ( element.Name() == "CreationTime" )
         {
            m_modelCreationTime = TimePoint( element.Text().Trimmed() );
            // initialize m_syncDataMaxCreationTime
            m_syncDataMaxCreationTime = m_modelCreationTime;
         }
         else if ( element.Name() == "ModelParameters" )
         {
            m_modelEachPierSide = element.AttributeValue( "PierSide" ) != "None";
            if ( element.AttributeValue( "PierSide" ) == "West" || element.AttributeValue( "PierSide" ) == "None" )
               *m_pointingModelWest = ParseListOfRealValues( element );
            else
               *m_pointingModelEast = ParseListOfRealValues( element );
         }
         else if ( element.Name() == "SyncDataList" )
            AlignmentModel::ParseSyncData(element);
         else
            WarnOnUnknownChildElement( element, "xtpm root" );
      }
      catch ( Exception& x )
      {
         try
         {
            throw XMLParseError( element, "Parsing " + element.Name() + " element", x.Message() );
         }
         catch ( Exception& x )
         {
            x.Show();
         }
      }
      catch ( ... )
      {
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::PrintParameterVector( Vector* parameters, double residual )
{
   m_console.WriteLn( String().Format( "<end><cbr>"
                                       "* Fitting residual ................................................ %+.2f ", residual ) );
   m_console.WriteLn( String().Format( "* Hour angle offset ............................................... %+.2f (arcmin)", (*parameters)[0]*scaleArcmin ) );
   m_console.WriteLn( String().Format( "* Declination offset .............................................. %+.2f (arcmin)", (*parameters)[1]*scaleArcmin ) );
   double poleSep = Sqrt( (*parameters)[4] * (*parameters)[4] + (*parameters)[5]* (*parameters)[5] );
   double poleAngle = ArcCos(- (*parameters)[4] / poleSep );
   m_console.WriteLn( String().Format( "* Angular separation between true and instrumental pole ........... %+.2f (arcmin)", poleSep*scaleArcmin ) );
   m_console.WriteLn( String().Format( "* Angle between meridian and line of true and instrumental pole ... %+.2f (deg)", Deg( poleAngle ) ) );
   m_console.WriteLn( String().Format( "* Mis-alignment of optical and mechanical axes .................... %+.2f (arcmin)", (*parameters)[2]*scaleArcmin ) );
   m_console.WriteLn( String().Format( "* Polar/declination axis non-orthogonality ........................ %+.2f (arcmin)", -(*parameters)[3]*scaleArcmin ) );
   m_console.WriteLn( String().Format( "* Tube flexure - droop away from zenith ........................... %+.2f (arcmin)", (*parameters)[6]*scaleArcmin ) );
   m_console.WriteLn( String().Format( "* Bending of declination axis ..................................... %+.2f (arcmin)", (*parameters)[8]*scaleArcmin ) );
   m_console.WriteLn( String().Format( "* Linear hour angle scale error ................................... %+.2f (arcmin)", (*parameters)[9]*scaleArcmin ) );
   m_console.WriteLn( String().Format( "* Quadratic hour angle scale error ................................ %+.2f (arcmin)", (*parameters)[10]*scaleArcmin ) );
}

// ----------------------------------------------------------------------------

void GeneralAnalyticalPointingModel::PrintParameters()
{
   m_console.NoteLn( "<end><cbr><br> Analytical Pointing Model Parameters" );
   m_console.WriteLn( "The parameters refer to the general analytical telescope pointing model as described by Marc W. Buie (2013)" );
   m_console.WriteLn( "in his paper http://www.boulder.swri.edu/~buie/idl/downloads/pointing/pointing.pdf." );
   if ( m_modelEachPierSide )
      m_console.WriteLn( "<br>* Pierside: West" );
   PrintParameterVector( m_pointingModelWest, m_residuals[0] );
   if ( m_modelEachPierSide )
   {
      m_console.WriteLn( "<br>* Pierside: East" );
      PrintParameterVector( m_pointingModelEast, m_residuals[1] );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Alignment.cpp - Released 2018-12-12T09:25:25Z
