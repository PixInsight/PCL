//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/AstrometricMetadata.cpp - Released 2018-12-12T09:24:30Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/AstrometricMetadata.h>
#include <pcl/ImageWindow.h>
#include <pcl/ProjectionFactory.h>
#include <pcl/XISF.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( ProjectionBase* projection,
                                          WorldTransformation* worldTransformation, int width, int height ) :
   m_projection( projection ),
   m_transformWI( worldTransformation ),
   m_width( width ),
   m_height( height )
{
   LinearTransformation linearTransIW = m_transformWI->ApproximateLinearTransform();
   double resx = Sqrt( linearTransIW.A00() * linearTransIW.A00() + linearTransIW.A01() * linearTransIW.A01() );
   double resy = Sqrt( linearTransIW.A10() * linearTransIW.A10() + linearTransIW.A11() * linearTransIW.A11() );
   m_resolution = (resx + resy)/2;
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Build( const FITSKeywordArray& keywords, const ByteArray& controlPoints, int width, int height )
{
   m_description.Destroy();

   WCSKeywords wcs( keywords );

   m_xpixsz = wcs.xpixsz();
   m_dateobs = wcs.dateobs;
   m_width = width;
   m_height = height;
   m_resolution = 0;

   if ( wcs.ctype1.StartsWith( "RA--" ) &&
        wcs.ctype2.StartsWith( "DEC-" ) &&
        wcs.crpix1.IsDefined() && wcs.crpix2.IsDefined() && wcs.crval1.IsDefined() && wcs.crval2.IsDefined() )
   {
      m_projection = ProjectionFactory::Create( wcs );

      LinearTransformation linearTransIW;
      if ( wcs.ExtractWorldTransformation( linearTransIW, m_height ) )
      {
         if ( !controlPoints.IsEmpty() )
            m_transformWI = new SplineWorldTransformation( controlPoints, linearTransIW );
         else
            m_transformWI = new LinearWorldTransformation( linearTransIW );

         double resx = Sqrt( linearTransIW.A00()*linearTransIW.A00() + linearTransIW.A01()*linearTransIW.A01() );
         double resy = Sqrt( linearTransIW.A10()*linearTransIW.A10() + linearTransIW.A11()*linearTransIW.A11() );
         m_resolution = (resx + resy)/2;
         m_useFocal = false;
         if ( m_xpixsz.IsDefined() )
            if ( m_xpixsz() > 0 )
               m_focal = FocalFromResolution( m_resolution );
      }
   }

   if ( m_transformWI.IsNull() )
   {
      if ( wcs.focallen.IsDefined() )
         if ( wcs.focallen() > 0 )
         {
            m_focal = wcs.focallen();
            m_useFocal = true;
         }
      if ( m_useFocal )
         if ( m_xpixsz.IsDefined() )
            if ( m_xpixsz() > 0 )
               m_resolution = ResolutionFromFocal( m_focal() );
   }
}

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( const ImageWindow& window )
{
   int width, height;
   View view = window.MainView();
   view.GetSize( width, height );

   FITSKeywordArray keywords = window.Keywords();

   ByteArray controlPoints;
   Variant v = view.PropertyValue( "Transformation_ImageToProjection" );
   if ( v.IsValid() )
      controlPoints = v.ToByteArray();

   Build( keywords, controlPoints, width, height );
}

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( XISFReader& reader )
{
   ImageInfo info = reader.ImageInfo();

   FITSKeywordArray keywords = reader.ReadFITSKeywords();

   ByteArray controlPoints;
   Variant v = reader.ReadImageProperty( "Transformation_ImageToProjection" );
   if ( v.IsValid() )
      controlPoints = v.ToByteArray();

   Build( keywords, controlPoints, info.width, info.height );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Write( ImageWindow& window, bool notify ) const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Write(): Invalid or uninitialized metadata." );

   View view = window.MainView();
   if( view.Width() != m_width || view.Height() != m_height )
      throw Error( "AstrometricMetadata::Write(): Metadata not compatible with the dimensions of the image." );

   FITSKeywordArray keywords = window.Keywords();
   UpdateBasicKeywords( keywords );
   UpdateWCSKeywords( keywords );
   window.SetKeywords( keywords );

   if ( m_focal.IsDefined() && m_focal() > 0 )
      view.SetStorablePropertyValue( "Instrument:Telescope:FocalLength", Round( m_focal()/1000, 6 ), notify );

   if ( m_xpixsz.IsDefined() && m_xpixsz() > 0 )
   {
      view.SetStorablePropertyValue( "Instrument:Sensor:XPixelSize", Round( m_xpixsz(), 3 ), notify );
      view.SetStorablePropertyValue( "Instrument:Sensor:YPixelSize", Round( m_xpixsz(), 3 ), notify );
   }

   DPoint pRD;
   if ( ImageToCelestial( pRD, DPoint( m_width/2.0, m_height/2.0 ) ) )
   {
      view.SetStorablePropertyValue( "Observation:Center:RA", pRD.x, notify );
      view.SetStorablePropertyValue( "Observation:Center:Dec", pRD.y, notify );
      view.SetStorablePropertyValue( "Observation:CelestialReferenceSystem", "ICRS", notify );
      view.SetStorablePropertyValue( "Observation:Equinox", 2000.0, notify );
      // The default reference point is the geometric center of the image.
      view.DeleteProperty( "Observation:Center:X", notify );
      view.DeleteProperty( "Observation:Center:Y", notify );
   }

   const SplineWorldTransformation* splineTransform = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
   if ( splineTransform != nullptr )
   {
      ByteArray data;
      splineTransform->Serialize( data );
      view.SetStorablePropertyValue( "Transformation_ImageToProjection", data, notify );
   }
   else
      view.DeleteProperty( "Transformation_ImageToProjection", notify );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Write( XISFWriter& writer ) const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Write(): Invalid or uninitialized metadata." );

   FITSKeywordArray keywords = writer.FITSKeywords();
   UpdateBasicKeywords( keywords );
   UpdateWCSKeywords( keywords );
   writer.WriteFITSKeywords( keywords );

   if ( m_focal.IsDefined() && m_focal() > 0 )
      writer.WriteImageProperty( "Instrument:Telescope:FocalLength", Round( m_focal()/1000, 6 ) );

   if ( m_xpixsz.IsDefined() && m_xpixsz() > 0 )
   {
      writer.WriteImageProperty( "Instrument:Sensor:XPixelSize", Round( m_xpixsz(), 3 ) );
      writer.WriteImageProperty( "Instrument:Sensor:YPixelSize", Round( m_xpixsz(), 3 ) );
   }

   DPoint pRD;
   if ( ImageToCelestial( pRD, DPoint( m_width/2.0, m_height/2.0 ) ) )
   {
      writer.WriteImageProperty( "Observation:Center:RA", pRD.x );
      writer.WriteImageProperty( "Observation:Center:Dec", pRD.y );
      writer.WriteImageProperty( "Observation:CelestialReferenceSystem", "ICRS" );
      writer.WriteImageProperty( "Observation:Equinox", 2000.0 );
      // The default reference point is the geometric center of the image.
      writer.RemoveImageProperty( "Observation:Center:X" );
      writer.RemoveImageProperty( "Observation:Center:Y" );
   }

   const SplineWorldTransformation* splineTransform = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
   if ( splineTransform != nullptr )
   {
      ByteArray data;
      splineTransform->Serialize( data );
      writer.WriteImageProperty( "Transformation_ImageToProjection", data );
   }
   else
      writer.RemoveImageProperty( "Transformation_ImageToProjection" );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Verify( double& ex, double& ey ) const
{
   try
   {
      if ( !IsValid() )
         throw Error( "Invalid or uninitialized metadata." );

      /*
       * Performs a full cycle transformation (image > celestial > image) and
       * reports the resulting absolute differences in pixels.
       */
      DPoint pI0( m_width/2.0, m_height/2.0 );
      DPoint pRD;
      if ( !ImageToCelestial( pRD, pI0 ) )
         throw Error( "Failed to perform ImageToCelestial() coordinate transformation." );
      DPoint pI1;
      if ( !CelestialToImage( pI1, pRD ) )
         throw Error( "Failed to perform CelestialToImage() coordinate transformation." );
      ex = Abs( pI0.x - pI1.x );
      ey = Abs( pI0.y - pI1.y );
   }
   catch ( const Exception& x )
   {
      throw Error( "AstrometricMetadata::Verify(): " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Validate( double tolerance ) const
{
   double ex, ey;
   Verify( ex, ey );
   if ( ex > tolerance || ey > tolerance )
      throw Error( "AstrometricMetadata::Validate(): Inconsistent coordinate transformation results." );
}

// ----------------------------------------------------------------------------

double AstrometricMetadata::Rotation( bool& flipped ) const
{
   if ( m_transformWI.IsNull() )
      throw Error( "Invalid call to AstrometricMetadata::Rotation(): No world transformation defined." );

   LinearTransformation linearTransIW = m_transformWI->ApproximateLinearTransform();
   double det = linearTransIW.A01() * linearTransIW.A10() - linearTransIW.A00() * linearTransIW.A11();
   double rotation = Deg( ArcTan( linearTransIW.A00() + linearTransIW.A01(), linearTransIW.A10() + linearTransIW.A11() ) ) + 135;
   if ( det > 0 )
      rotation = -90 - rotation;
   if ( rotation < -180 )
      rotation += 360;
   if ( rotation > 180 )
      rotation -= 360;
   flipped = det > 0;
   return rotation;
}

// ----------------------------------------------------------------------------

String AstrometricMetadata::Summary() const
{
   if ( !IsValid() )
      throw Error( "Invalid call to AstrometricMetadata::Summary(): No astrometric solution." );

   UpdateDescription();

   String summary;
   summary    << "Reference matrix (world[ra,dec] = matrix * image[x,y]):" << '\n'
              << m_description->referenceMatrix << '\n'
              << "WCS transformation ...... " << m_description->wcsTransformationType << '\n'
              << "Projection .............. " << m_description->projectionName << '\n'
              << "Projection origin ....... " << m_description->projectionOrigin << '\n'
              << "Resolution .............. " << m_description->resolution << '\n'
              << "Rotation ................ " << m_description->rotation << '\n'
              << "Transformation errors ... " << m_description->transformationErrors << '\n';

   if ( !m_description->observationDate.IsEmpty() )
      summary << "Observation date ........ " << m_description->observationDate << '\n';

   if ( !m_description->focalDistance.IsEmpty() )
      summary << "Focal distance .......... " << m_description->focalDistance << '\n';

   if ( !m_description->pixelSize.IsEmpty() )
      summary << "Pixel size .............. " << m_description->pixelSize << '\n';

   summary    << "Field of view ........... " << m_description->fieldOfView << '\n'
              << "Image center ............ " << m_description->centerCoordinates << '\n'
              << "Image bounds:" << '\n'
              << "   top-left ............. " << m_description->topLeftCoordinates << '\n'
              << "   top-right ............ " << m_description->topRightCoordinates << '\n'
              << "   bottom-left .......... " << m_description->bottomLeftCoordinates << '\n'
              << "   bottom-right ......... " << m_description->bottomRightCoordinates << '\n';

   return summary;
}

// ----------------------------------------------------------------------------

static void ModifyKeyword( FITSKeywordArray& keywords, IsoString name, IsoString value, IsoString comment )
{
   for ( FITSHeaderKeyword& keyword : keywords )
      if ( keyword.name == name )
      {
         keyword.value = value;
         keyword.comment = comment;
         return;
      }

   keywords << FITSHeaderKeyword( name, value, comment );
}

static void RemoveKeyword( FITSKeywordArray& keywords, IsoString name )
{
   keywords.Remove( FITSHeaderKeyword( name ), []( const FITSHeaderKeyword& k1, const FITSHeaderKeyword& k2 )
                                               { return k1.name == k2.name; } );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::UpdateBasicKeywords( FITSKeywordArray& keywords ) const
{
   if ( m_focal.IsDefined() && m_focal() > 0 )
      ModifyKeyword( keywords, "FOCALLEN", IsoString().Format( "%.3f", m_focal() ), "Focal length (mm)" );
   else
      RemoveKeyword( keywords, "FOCALLEN" );

   if ( m_xpixsz.IsDefined() )
      if ( m_xpixsz() > 0 )
      {
         ModifyKeyword( keywords, "XPIXSZ", IsoString().Format( "%.3f", m_xpixsz() ), "Pixel size, X-axis (um)" );
         ModifyKeyword( keywords, "YPIXSZ", IsoString().Format( "%.3f", m_xpixsz() ), "Pixel size, Y-axis (um)" );
      }

   /*
    * ### N.B.: It is unclear if writing the center coordinates to nonstandard
    * OBJCTRA/OBJCTDEC keywords is really a good idea. However, this is
    * customary practice...
    */
   DPoint pRD;
   if ( ImageToCelestial( pRD, DPoint( m_width/2.0, m_height/2.0 ) ) )
   {
      ModifyKeyword( keywords, "OBJCTRA",
            '\'' + IsoString::ToSexagesimal( pRD.x/15, RAConversionOptions( 3/*precision*/, 0/*width*/ ) ) + '\'',
            "Right ascension of the center of the image" );
      ModifyKeyword( keywords, "OBJCTDEC",
            '\'' + IsoString::ToSexagesimal( pRD.y, DecConversionOptions( 2/*precision*/, 0/*width*/ ) ) + '\'',
            "Declination of the center of the image" );
   }
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::UpdateWCSKeywords( FITSKeywordArray& keywords ) const
{
   RemoveKeyword( keywords, "EQUINOX" );
   RemoveKeyword( keywords, "CTYPE1" );
   RemoveKeyword( keywords, "CTYPE2" );
   RemoveKeyword( keywords, "CRPIX1" );
   RemoveKeyword( keywords, "CRPIX2" );
   RemoveKeyword( keywords, "CRVAL1" );
   RemoveKeyword( keywords, "CRVAL2" );
   RemoveKeyword( keywords, "PV1_1" );
   RemoveKeyword( keywords, "PV1_2" );
   RemoveKeyword( keywords, "LONPOLE" );
   RemoveKeyword( keywords, "LATPOLE" );
   RemoveKeyword( keywords, "CD1_1" );
   RemoveKeyword( keywords, "CD1_2" );
   RemoveKeyword( keywords, "CD2_1" );
   RemoveKeyword( keywords, "CD2_2" );
   RemoveKeyword( keywords, "REFSPLINE" );
   RemoveKeyword( keywords, "CDELT1" );
   RemoveKeyword( keywords, "CDELT2" );
   RemoveKeyword( keywords, "CROTA1" );
   RemoveKeyword( keywords, "CROTA2" );

   if ( IsValid() )
   {
      WCSKeywords wcs = GetWCSvalues();

      keywords << FITSHeaderKeyword( "EQUINOX", "2000.0", "Coordinates referred to ICRS / J2000.0" )
               << FITSHeaderKeyword( "CTYPE1", wcs.ctype1, "Axis1 projection: " + m_projection->Name() )
               << FITSHeaderKeyword( "CTYPE2", wcs.ctype2, "Axis2 projection: " + m_projection->Name() )
               << FITSHeaderKeyword( "CRPIX1", IsoString().Format( "%.8g", wcs.crpix1() ), "Axis1 reference pixel" )
               << FITSHeaderKeyword( "CRPIX2", IsoString().Format( "%.8g", wcs.crpix2() ), "Axis2 reference pixel" );

      if ( wcs.crval1.IsDefined() )
         keywords << FITSHeaderKeyword( "CRVAL1", IsoString().Format( "%.16g", wcs.crval1() ), "Axis1 reference value" );
      if ( wcs.crval2.IsDefined() )
         keywords << FITSHeaderKeyword( "CRVAL2", IsoString().Format( "%.16g", wcs.crval2() ), "Axis2 reference value" );
      if ( wcs.pv1_1.IsDefined() )
         keywords << FITSHeaderKeyword( "PV1_1", IsoString().Format( "%.16g", wcs.pv1_1() ), "Native longitude of the reference point (deg)" );
      if ( wcs.pv1_2.IsDefined() )
         keywords << FITSHeaderKeyword( "PV1_2", IsoString().Format( "%.16g", wcs.pv1_2() ), "Native latitude of the reference point (deg)" );
      if ( wcs.lonpole.IsDefined() )
         keywords << FITSHeaderKeyword( "LONPOLE", IsoString().Format( "%.16g", wcs.lonpole() ), "Longitude of the celestial pole (deg)" );
      if ( wcs.latpole.IsDefined() )
         keywords << FITSHeaderKeyword( "LATPOLE", IsoString().Format( "%.16g", wcs.latpole() ), "Latitude of the celestial pole (deg)" );

      keywords << FITSHeaderKeyword( "CD1_1", IsoString().Format( "%.16g", wcs.cd1_1() ), "Scale matrix (1,1)" )
               << FITSHeaderKeyword( "CD1_2", IsoString().Format( "%.16g", wcs.cd1_2() ), "Scale matrix (1,2)" )
               << FITSHeaderKeyword( "CD2_1", IsoString().Format( "%.16g", wcs.cd2_1() ), "Scale matrix (2,1)" )
               << FITSHeaderKeyword( "CD2_2", IsoString().Format( "%.16g", wcs.cd2_2() ), "Scale matrix (2,2)" );

      if ( HasSplineWorldTransformation() )
         keywords << FITSHeaderKeyword( "REFSPLINE", "T", "Spline-based astrometric solution available" );

      // AIPS keywords (CDELT1, CDELT2, CROTA1, CROTA2)
      keywords << FITSHeaderKeyword( "CDELT1", IsoString().Format( "%.16g", wcs.cdelt1() ), "Axis1 scale" )
               << FITSHeaderKeyword( "CDELT2", IsoString().Format( "%.16g", wcs.cdelt2() ), "Axis2 scale" )
               << FITSHeaderKeyword( "CROTA1", IsoString().Format( "%.16g", wcs.crota1() ), "Axis1 rotation angle (deg)" )
               << FITSHeaderKeyword( "CROTA2", IsoString().Format( "%.16g", wcs.crota2() ), "Axis2 rotation angle (deg)" );
   }
}

// ----------------------------------------------------------------------------

static void ModifyProperty( PropertyArray& properties, const IsoString& id, const Variant& value )
{
   PropertyArray::iterator i = properties.Search( id );
   if ( i != properties.End() )
      i->SetValue( value );
   else
      properties << Property( id, value );
}

static void RemoveProperty( PropertyArray& properties, const IsoString& id )
{
   properties.Remove( Property( id ) );
}

void AstrometricMetadata::UpdateProperties( PropertyArray& properties ) const
{
   if ( IsValid() )
   {
      if ( m_focal.IsDefined() && m_focal() > 0 )
         ModifyProperty( properties, "Instrument:Telescope:FocalLength", Round( m_focal()/1000, 6 ) );

      if ( m_xpixsz.IsDefined() && m_xpixsz() > 0 )
      {
         ModifyProperty( properties, "Instrument:Sensor:XPixelSize", Round( m_xpixsz(), 3 ) );
         ModifyProperty( properties, "Instrument:Sensor:YPixelSize", Round( m_xpixsz(), 3 ) );
      }

      DPoint pRD;
      if ( ImageToCelestial( pRD, DPoint( m_width/2.0, m_height/2.0 ) ) )
      {
         ModifyProperty( properties, "Observation:Center:RA", pRD.x );
         ModifyProperty( properties, "Observation:Center:Dec", pRD.y );
         ModifyProperty( properties, "Observation:CelestialReferenceSystem", "ICRS" );
         ModifyProperty( properties, "Observation:Equinox", 2000.0 );
         // The default reference point is the geometric center of the image.
         RemoveProperty( properties, "Observation:Center:X" );
         RemoveProperty( properties, "Observation:Center:Y" );
      }

      const SplineWorldTransformation* splineTransform = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
      if ( splineTransform != nullptr )
      {
         ByteArray data;
         splineTransform->Serialize( data );
         ModifyProperty( properties, "Transformation_ImageToProjection", data );
      }
      else
         RemoveProperty( properties, "Transformation_ImageToProjection" );
   }
   else
      RemoveProperty( properties, "Transformation_ImageToProjection" );
}

// ----------------------------------------------------------------------------

WCSKeywords AstrometricMetadata::GetWCSvalues() const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::GetWCSvalues(): Invalid or uninitialized metadata" );

   LinearTransformation trans_F_I( 1,  0, -0.5,
                                   0, -1,  m_height+0.5 );
   LinearTransformation trans_I_W = m_transformWI->ApproximateLinearTransform();
   LinearTransformation trans_F_W = trans_I_W.Multiply( trans_F_I );

   WCSKeywords wcs;
   m_projection->GetWCS( wcs );

   wcs.cd1_1 = trans_F_W.A00();
   wcs.cd1_2 = trans_F_W.A01();
   wcs.cd2_1 = trans_F_W.A10();
   wcs.cd2_2 = trans_F_W.A11();

   DPoint orgF = trans_F_W.TransformInverse( DPoint( 0 ) );
   wcs.crpix1 = orgF.x;
   wcs.crpix2 = orgF.y;

   /*
    * CDELT1, CDELT2 and CROTA2 are computed using the formulas in section 6.2
    * of http://fits.gsfc.nasa.gov/fits_wcs.html "Representations of celestial
    * coordinates in FITS".
    */
   double rot1, rot2;
   if ( wcs.cd2_1() > 0 )
      rot1 = ArcTan( wcs.cd2_1(), wcs.cd1_1() );
   else if ( wcs.cd2_1() < 0 )
      rot1 = ArcTan( -wcs.cd2_1(), -wcs.cd1_1() );
   else
      rot1 = 0;

   if ( wcs.cd1_2() > 0 )
      rot2 = ArcTan( wcs.cd1_2(), -wcs.cd2_2() );
   else if ( wcs.cd1_2() < 0 )
      rot2 = ArcTan( -wcs.cd1_2(), wcs.cd2_2() );
   else
      rot2 = 0;

   double rot = (rot1 + rot2)/2;
   rot2 = rot1 = rot;
   double sinrot, cosrot;
   SinCos( rot, sinrot, cosrot );
   if ( Abs( cosrot ) > Abs( sinrot ) )
   {
      wcs.cdelt1 =  wcs.cd1_1()/cosrot;
      wcs.cdelt2 =  wcs.cd2_2()/cosrot;
   }
   else
   {
      wcs.cdelt1 =  wcs.cd2_1()/sinrot;
      wcs.cdelt2 = -wcs.cd1_2()/sinrot;
   }

   wcs.crota1 = Deg( rot1 );
   wcs.crota2 = Deg( rot2 );

   return wcs;
}

// ----------------------------------------------------------------------------

static String FieldString( double field )
{
   int sign, s1, s2; double s3;
   IsoString::ToSexagesimal( field ).ParseSexagesimal( sign, s1, s2, s3 );
   if ( s1 > 0 )
      return String().Format( "%dd %d' %.1f\"", s1, s2, s3 );
   if ( s2 > 0 )
      return String().Format( "%d' %.1f\"", s2, s3 );
   return String().Format( "%.2f\"", s3 );
}

static String CelestialToString( const DPoint& pRD )
{
   double ra = pRD.x;
   if ( ra < 0 )
      ra += 360;
   return String()
      << "RA: "    << String().ToSexagesimal( ra/15, RAConversionOptions( 3/*precision*/ ) )
      << "  Dec: " << String().ToSexagesimal( pRD.y, DecConversionOptions( 2/*precision*/ ) );
}

static String ImageToCelestialToString( const AstrometricMetadata* A, const DPoint& pI )
{
   DPoint pRD;
   if ( A->ImageToCelestial( pRD, pI ) )
      return CelestialToString( pRD );
   return "------";
}

void AstrometricMetadata::UpdateDescription() const
{
   if ( m_description.IsNull() )
      if ( IsValid() )
      {
         LinearTransformation linearTransIW = m_transformWI->ApproximateLinearTransform();
         DPoint projOrgPx = linearTransIW.TransformInverse( DPoint( 0 ) );
         DPoint projOrgRD = m_projection->ProjectionOrigin();
         bool flipped;
         double rotation = Rotation( flipped );
         double ex, ey;
         Verify( ex, ey );

         m_description = new DescriptionItems;

         m_description->referenceMatrix = linearTransIW.ToString();
         m_description->wcsTransformationType = HasSplineWorldTransformation() ? "2-D surface splines" : "linear";
         m_description->projectionName = m_projection->Name();
         m_description->projectionOrigin = String().Format( "[%.6f %.6f]px", projOrgPx.x, projOrgPx.y )
                                             << " -> [" << CelestialToString( projOrgRD ) << ']';
         m_description->resolution = String().Format( "%.3f arcsec/px", m_resolution*3600 );
         m_description->rotation = String().Format( "%.3f deg", rotation ) << (flipped ? " (flipped)" : "");
         m_description->transformationErrors = String().Format( "ex=%.3g ey=%.3g px", ex, ey );

         if ( m_dateobs.IsDefined() )
            m_description->observationDate = TimePoint( m_dateobs() ).ToString( "%Y-%M-%D %h:%m:%s0" );

         if ( m_xpixsz.IsDefined() )
            if ( m_xpixsz() > 0 )
               if ( m_focal.IsDefined() )
               {
                  m_description->focalDistance = String().Format( "%.2f mm", m_focal() );
                  m_description->pixelSize = String().Format( "%.2f um", m_xpixsz() );
               }

         m_description->fieldOfView = FieldString( m_width*m_resolution ) << " x " << FieldString( m_height*m_resolution );
         m_description->centerCoordinates = ImageToCelestialToString( this, DPoint( m_width/2.0, m_height/2.0 ) );
         m_description->topLeftCoordinates = ImageToCelestialToString( this, DPoint( 0, 0 ) );
         m_description->topRightCoordinates = ImageToCelestialToString( this, DPoint( m_width, 0 ) );
         m_description->bottomLeftCoordinates = ImageToCelestialToString( this, DPoint( 0, m_height ) );
         m_description->bottomRightCoordinates = ImageToCelestialToString( this, DPoint( m_width, m_height ) );
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricMetadata.cpp - Released 2018-12-12T09:24:30Z
